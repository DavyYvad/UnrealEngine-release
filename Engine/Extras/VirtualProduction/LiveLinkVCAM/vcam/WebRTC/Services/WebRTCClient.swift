//
//  WebRTCClient.swift
//  WebRTC
//
//  Created by Stasel on 20/05/2018.
//  Heavily modified by Luke Bermingham 2022
//  Copyright © 2018 Stasel. All rights reserved.
//

import Foundation
import WebRTC

protocol WebRTCClientDelegate: AnyObject {
    func webRTCClient(_ client: WebRTCClient, didDiscoverLocalCandidate candidate: RTCIceCandidate)
    func webRTCClient(_ client: WebRTCClient, didChangeConnectionState state: RTCIceConnectionState)
    func webRTCClient(_ client: WebRTCClient, didReceiveData data: Data)
    func webRTCClient(_ client: WebRTCClient, onStartReceiveVideo video: RTCVideoTrack)
}

final class WebRTCClient: NSObject {
    
    // The `RTCPeerConnectionFactory` is in charge of creating new RTCPeerConnection instances.
    // A new RTCPeerConnection should be created every new call, but the factory is shared.
    private static let factory: RTCPeerConnectionFactory = {
        RTCInitializeSSL()
        let videoEncoderFactory = RTCDefaultVideoEncoderFactory()
        let videoDecoderFactory = RTCDefaultVideoDecoderFactory()

        let fieldTrials = [
            "WebRTC-MaxPacketBufferSize": "8192",
            "WebRTC-ForcePlayoutDelay" : "min_ms:1,max_ms:1",
            "WebRTC-ZeroPlayoutDelay" : "min_pacing:4ms,max_decode_queue_size:1"
        ]
        RTCInitFieldTrialDictionary(fieldTrials)


        return RTCPeerConnectionFactory(encoderFactory: videoEncoderFactory, decoderFactory: videoDecoderFactory)
    }()
    
    weak var delegate: WebRTCClientDelegate?

    private let rtcAudioSession =  RTCAudioSession.sharedInstance()
    private let audioQueue = DispatchQueue(label: "audio")
    private let mediaConstrains = [kRTCMediaConstraintsOfferToReceiveAudio: kRTCMediaConstraintsValueTrue,
                                   kRTCMediaConstraintsOfferToReceiveVideo: kRTCMediaConstraintsValueTrue]    
    
    private var peerConnection: RTCPeerConnection?
    private var remoteVideoTrack: RTCVideoTrack?
    private var dataChannel: RTCDataChannel?
    
    required override init() {
        super.init()
    }
    
    deinit {
        Log.info("WebRTCClient destructed.")
    }
    
    // MARK: PeerConnection Config
    func setupPeerConnection(rtcConfiguration: RTCConfiguration) {
        
        // Define media constraints. DtlsSrtpKeyAgreement is required to be true to be able to connect with web browsers.
        let constraints = RTCMediaConstraints(mandatoryConstraints: nil,
                                              optionalConstraints: nil /*["DtlsSrtpKeyAgreement":kRTCMediaConstraintsValueTrue]*/)
        
        guard let pc = WebRTCClient.factory.peerConnection(with: rtcConfiguration, constraints: constraints, delegate: nil) else {
            fatalError("Could not create new RTCPeerConnection")
        }
        
        self.peerConnection = pc
        
        self.configureAudioSession()
        self.peerConnection!.delegate = self
    }
    
    func close() {
        if let pc = self.peerConnection {
            pc.close()
            self.peerConnection = nil
        }
        self.remoteVideoTrack = nil
        self.dataChannel = nil
    }

    func stats(_ completionHandler : @escaping RTCStatisticsCompletionHandler) {
        self.peerConnection?.statistics(completionHandler: completionHandler)
    }
    
    func hasPeerConnnection() -> Bool {
        if let _ = self.peerConnection {
            return true
        } else {
            return false
        }
    }
    
    func getRemoteVideoTrack() -> RTCVideoTrack? {
        return self.remoteVideoTrack
    }
    
    // MARK: Signaling
    func offer(completion: @escaping (_ sdp: RTCSessionDescription) -> Void) {
        
        if let peerConnection = peerConnection {
            let constrains = RTCMediaConstraints(mandatoryConstraints: self.mediaConstrains,
                                                 optionalConstraints: nil)
            
            peerConnection.offer(for: constrains) { (sdp, error) in
                guard let sdp = sdp else {
                    return
                }
                
                peerConnection.setLocalDescription(sdp, completionHandler: { (error) in
                    completion(sdp)
                })
            }
        }
        else {
            debugPrint("Peer connection not setup yet, cannot handle an offer yet.")
        }
    }
    
    func answer(completion: @escaping (_ sdp: RTCSessionDescription) -> Void)  {
        
        if let peerConnection = peerConnection {
            let constrains = RTCMediaConstraints(mandatoryConstraints: self.mediaConstrains,
                                                 optionalConstraints: nil)
            peerConnection.answer(for: constrains) { (sdp, error) in
                guard let sdp = sdp else {
                    return
                }
                
                peerConnection.setLocalDescription(sdp, completionHandler: { (error) in
                    completion(sdp)
                })
            }
        } else {
            debugPrint("Peer connection not setup yet, cannot handle answer yet.")
        }
        
    }
    
    func handleRemoteSdp(remoteSdp: RTCSessionDescription, completion: @escaping (Error?) -> ()) {
        if let peerConnection = peerConnection {
            peerConnection.setRemoteDescription(remoteSdp, completionHandler: completion)
        } else {
            debugPrint("Peer connection not setup yet, cannot handle setting remote sdp yet.")
        }
    }
    
    func handleRemoteCandidate(remoteCandidate: RTCIceCandidate, completion: @escaping (Error?) -> ()) {
        if let peerConnection = peerConnection {
            peerConnection.add(remoteCandidate, completionHandler: completion)
        } else {
            debugPrint("Peer connection not setup yet, cannot handle setting remote candidate yet.")
        }
    }
    
    func sendRequestQualityControl() {
        let bytes: [UInt8] = [PixelStreamingToStreamerMessage.RequestQualityControl.rawValue]
        Log.info("Sending quality control request")
        self.sendData(Data(bytes))
    }
    
    func sendRequestKeyFrame(){
        let bytes: [UInt8] = [PixelStreamingToStreamerMessage.IFrameRequest.rawValue]
        Log.info("Sending keyframe request")
        self.sendData(Data(bytes))
    }
    
    func sendDeviceResolution() {
        let bounds = UIScreen.main.nativeBounds
        
        // the device nativeBounds are always in portrait orientation, so we swap them here because we always run
        // in landscape.
        let resolutionCommand = PixelStreamingToStreamerResolutionCommand(width: Int(bounds.height), height: Int(bounds.width))
        
        do {
            let jsonData = try JSONEncoder().encode(resolutionCommand)
            let jsonString = String(data: jsonData, encoding: .utf8)!
            
            var bytes: [UInt8] = []
            // Append the message type
            bytes.append(PixelStreamingToStreamerMessage.Command.rawValue)
            bytes.append(contentsOf: UInt16(jsonString.count).toBytes())
            // Append each character in the json string represented by its UTF16 character code
            for char in jsonString {
                let charCode = char.utf16[char.utf16.index(char.utf16.startIndex, offsetBy: 0)]
                bytes.append(contentsOf: charCode.toBytes())
            }
            
            Log.info("Sending remote resolution")
            self.sendData(Data(bytes))
        } catch {
            print(error)
        }
    }
    
}

// MARK: Data Channels
extension WebRTCClient: RTCDataChannelDelegate {
    
    func sendData(_ data: Data) {
        let buffer = RTCDataBuffer(data: data, isBinary: true)
        self.dataChannel?.sendData(buffer)
    }
    
    func dataChannelDidChangeState(_ dataChannel: RTCDataChannel) {
        debugPrint("dataChannel did change state: \(dataChannel.readyState)")
    }
    
    func dataChannel(_ dataChannel: RTCDataChannel, didReceiveMessageWith buffer: RTCDataBuffer) {
        self.delegate?.webRTCClient(self, didReceiveData: buffer.data)
    }
}

// MARK: PeerConnection
extension WebRTCClient: RTCPeerConnectionDelegate {
    
    func peerConnection(_ peerConnection: RTCPeerConnection, didChange stateChanged: RTCSignalingState) {
        debugPrint("peerConnection new signaling state: \(stateChanged)")
    }
    
    func peerConnection(_ peerConnection: RTCPeerConnection, didAdd stream: RTCMediaStream) {
        debugPrint("peerConnection did add stream. Id=\(stream.streamId)")
    }
    
    func peerConnection(_ peerConnection: RTCPeerConnection, didRemove stream: RTCMediaStream) {
        debugPrint("peerConnection did remove stream")
    }
    
    func peerConnectionShouldNegotiate(_ peerConnection: RTCPeerConnection) {
        debugPrint("peerConnection should negotiate")
    }
    
    func peerConnection(_ peerConnection: RTCPeerConnection, didChange newState: RTCIceConnectionState) {
        debugPrint("peerConnection new connection state: \(newState)")
        self.delegate?.webRTCClient(self, didChangeConnectionState: newState)
    }
    
    func peerConnection(_ peerConnection: RTCPeerConnection, didChange newState: RTCIceGatheringState) {
        debugPrint("peerConnection new gathering state: \(newState)")
    }
    
    func peerConnection(_ peerConnection: RTCPeerConnection, didGenerate candidate: RTCIceCandidate) {
        self.delegate?.webRTCClient(self, didDiscoverLocalCandidate: candidate)
    }
    
    func peerConnection(_ peerConnection: RTCPeerConnection, didRemove candidates: [RTCIceCandidate]) {
        debugPrint("peerConnection did remove candidate(s)")
    }
    
    func peerConnection(_ peerConnection: RTCPeerConnection, didOpen dataChannel: RTCDataChannel) {
        debugPrint("peerConnection did open data channel")
        
        // Store datachannel internally for message sending and set datachannel delegate so we can react to its events
        dataChannel.delegate = self
        self.dataChannel = dataChannel
        
        // Send the device resolution over the datachannel
        self.sendDeviceResolution()
    }
    
    func peerConnection(_ peerConnection: RTCPeerConnection, didAdd rtpReceiver: RTCRtpReceiver, streams mediaStreams: [RTCMediaStream]) {
        debugPrint("Added rtp receiver id=\(rtpReceiver.receiverId) trackId=\(rtpReceiver.track?.trackId ?? "none")")
    }
    
    func peerConnection(_ peerConnection: RTCPeerConnection, didStartReceivingOn transceiver: RTCRtpTransceiver) {
        
        var directionStr = ""
        switch transceiver.direction {
        case RTCRtpTransceiverDirection.inactive:
            directionStr = "inactive"
        case RTCRtpTransceiverDirection.recvOnly:
            directionStr = "recvonly"
        case RTCRtpTransceiverDirection.sendOnly:
            directionStr = "sendonly"
        case RTCRtpTransceiverDirection.sendRecv:
            directionStr = "sendrecv"
        case RTCRtpTransceiverDirection.stopped:
            directionStr = "stopped"
        @unknown default:
            directionStr = "unknown"
        }
        
        switch transceiver.mediaType {
        case .video:
            debugPrint("Started recieving video. Direction= \(directionStr)")
            transceiver.receiver.track?.isEnabled = true
            
            self.remoteVideoTrack = transceiver.receiver.track as? RTCVideoTrack
            self.delegate?.webRTCClient(self, onStartReceiveVideo: self.remoteVideoTrack!)
            
        case .audio:
            // disable audio sending
            transceiver.sender.track?.isEnabled = false
            debugPrint("Started receiving audio. Direction=\(directionStr)")
        case .data:
            debugPrint("Started receiving data channel data.")
        case .unsupported:
            debugPrint("Start receiving on unknown transceiver type - this is probably an error.")
        @unknown default:
            debugPrint("Unknown transceiver type.")
        }
    }
    
}

// MARK: Audio control
extension WebRTCClient {
    
    private func configureAudioSession() {
        self.rtcAudioSession.lockForConfiguration()
        do {
            try self.rtcAudioSession.setCategory(AVAudioSession.Category.ambient) /* Playback only */
            try self.rtcAudioSession.setMode(AVAudioSession.Mode.default) /* Mod.rawValuee to default mode */
        } catch let error {
            debugPrint("Error changeing AVAudioSession category: \(error)")
        }
        self.rtcAudioSession.unlockForConfiguration()
    }
    
    // Fallback to the default playing device: headphones/bluetooth/ear speaker
    func speakerOff() {
        self.audioQueue.async { [weak self] in
            guard let self = self else {
                return
            }
            
            self.rtcAudioSession.lockForConfiguration()
            do {
                try self.rtcAudioSession.overrideOutputAudioPort(.none)
                try self.rtcAudioSession.setActive(false)
            } catch let error {
                debugPrint("Error setting AVAudioSession category: \(error)")
            }
            self.rtcAudioSession.unlockForConfiguration()
        }
    }
    
    // Force speaker
    func speakerOn() {
        self.audioQueue.async { [weak self] in
            guard let self = self else {
                return
            }
            
            self.rtcAudioSession.lockForConfiguration()
            do {
                try self.rtcAudioSession.overrideOutputAudioPort(.speaker)
                try self.rtcAudioSession.setActive(true)
            } catch let error {
                debugPrint("Couldn't force audio to speaker: \(error)")
            }
            self.rtcAudioSession.unlockForConfiguration()
        }
    }

}

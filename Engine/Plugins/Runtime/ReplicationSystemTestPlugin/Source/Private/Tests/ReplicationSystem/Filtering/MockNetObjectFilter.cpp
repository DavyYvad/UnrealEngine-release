// Copyright Epic Games, Inc. All Rights Reserved.

#include "MockNetObjectFilter.h"
#include "Containers/ArrayView.h"
#include "TestFilteringObject.h"

#include "Iris/ReplicationSystem/ReplicationSystem.h"
#include "Iris/ReplicationSystem/ReplicationSystemInternal.h"

UMockNetObjectFilter::UMockNetObjectFilter()
: CallStatus({})
, AddedCount(0U)
{
}

void UMockNetObjectFilter::OnInit(FNetObjectFilterInitParams& Params)
{
	++CallStatus.CallCounts.Init;

	CallStatus.SuccessfulCallCounts.Init += Cast<UMockNetObjectFilterConfig>(Params.Config) != nullptr;

	AddedObjectIndices.Init(Params.MaxObjectCount);
	AddedConnectionIndices.Init(Params.MaxConnectionCount + 1);
}

void UMockNetObjectFilter::AddConnection(uint32 ConnectionId)
{
	++CallStatus.CallCounts.AddConnection;

	CallStatus.SuccessfulCallCounts.AddConnection += ConnectionId != 0U;

	AddedConnectionIndices.SetBit(ConnectionId);
}

void UMockNetObjectFilter::RemoveConnection(uint32 ConnectionId)
{
	++CallStatus.CallCounts.RemoveConnection;

	CallStatus.SuccessfulCallCounts.RemoveConnection += AddedConnectionIndices.GetBit(ConnectionId);
	AddedConnectionIndices.ClearBit(ConnectionId);
}

bool UMockNetObjectFilter::AddObject(uint32 ObjectIndex, FNetObjectFilterAddObjectParams& Params)
{
	++CallStatus.CallCounts.AddObject;
	if (CallSetup.AddObject.bReturnValue)
	{
		bool bIsProperCall = true;

		// Validate the passed info is zeroed
		bIsProperCall &= Params.OutInfo.Data[0] == 0;
		bIsProperCall &= Params.OutInfo.Data[1] == 0;
		bIsProperCall &= Params.OutInfo.Data[2] == 0;
		bIsProperCall &= Params.OutInfo.Data[3] == 0;
		
		CallStatus.SuccessfulCallCounts.AddObject += bIsProperCall;

		// Store object index in info so we can check it's passed to RemoveObject
		Params.OutInfo.Data[0] = uint16(ObjectIndex);
		Params.OutInfo.Data[1] = uint16(ObjectIndex);
		Params.OutInfo.Data[2] = uint16(ObjectIndex);
		Params.OutInfo.Data[3] = uint16(ObjectIndex);

		AddedObjectIndices.SetBit(ObjectIndex);
		++AddedCount;
	}

	return CallSetup.AddObject.bReturnValue;
}

void UMockNetObjectFilter::RemoveObject(uint32 ObjectIndex, const FNetObjectFilteringInfo& Info)
{
	++CallStatus.CallCounts.RemoveObject;

	// If this object wasn't added to us it shouldn't be removed either.
	if (AddedObjectIndices.GetBit(ObjectIndex))
	{
		bool bIsProperCall = true;

		// Validate the passed info is as we set it
		bIsProperCall &= Info.Data[0] == uint16(ObjectIndex);
		bIsProperCall &= Info.Data[1] == uint16(ObjectIndex);
		bIsProperCall &= Info.Data[2] == uint16(ObjectIndex);
		bIsProperCall &= Info.Data[3] == uint16(ObjectIndex);

		CallStatus.SuccessfulCallCounts.RemoveObject += bIsProperCall;

		AddedObjectIndices.ClearBit(ObjectIndex);
		--AddedCount;
	}
}

void UMockNetObjectFilter::UpdateObjects(FNetObjectFilterUpdateParams& Params)
{
	++CallStatus.CallCounts.UpdateObjects;

	bool bIsProperCall = Params.ObjectCount <= AddedCount;
	for (uint32 ObjectIndex : MakeArrayView(Params.ObjectIndices, Params.ObjectCount))
	{
		bIsProperCall = bIsProperCall && AddedObjectIndices.GetBit(ObjectIndex);
	}

	CallStatus.SuccessfulCallCounts.UpdateObjects += bIsProperCall;
}

void UMockNetObjectFilter::PreFilter(FNetObjectPreFilteringParams& Params)
{
	++CallStatus.CallCounts.PreFilter;

	constexpr bool bIsProperCall = true;
	CallStatus.SuccessfulCallCounts.PreFilter += bIsProperCall;
}

void UMockNetObjectFilter::Filter(FNetObjectFilteringParams& Params)
{
	++CallStatus.CallCounts.Filter;

	bool bIsProperCall = true;
	UE::Net::FNetBitArrayView::ForAllExclusiveBits(Params.FilteredObjects, MakeNetBitArrayView(AddedObjectIndices), [](...) {}, [&bIsProperCall](...) {bIsProperCall = false; });
	CallStatus.SuccessfulCallCounts.Filter += bIsProperCall;

	if (CallSetup.Filter.bFilterOutByDefault)
	{
		Params.OutAllowedObjects.Reset();
	}
	else
	{
		Params.OutAllowedObjects.Copy(MakeNetBitArrayView(AddedObjectIndices));
	}
}

void UMockNetObjectFilter::PostFilter(FNetObjectPostFilteringParams& Params)
{
	++CallStatus.CallCounts.PostFilter;

	constexpr bool bIsProperCall = true;
	CallStatus.SuccessfulCallCounts.PostFilter += bIsProperCall;
}


//**************************************************************************************************
// UMockNetObjectFilterUsingFragmentData
//**************************************************************************************************

void UMockNetObjectFilterUsingFragmentData::OnInit(FNetObjectFilterInitParams& Params)
{
	Super::OnInit(Params);

	SetupFilterType(ENetFilterType::PostPoll_FragmentBased);
}

bool UMockNetObjectFilterUsingFragmentData::AddObject(uint32 ObjectIndex, FNetObjectFilterAddObjectParams& Params)
{
	bool bResult = Super::AddObject(ObjectIndex, Params);

	if (bResult)
	{
		// Check if object has the NetTest_FilterOut RepTag which says whether the object should be filtered out or not.
		UE::Net::FRepTagFindInfo RepTagInfo;
		if (UE::Net::FindRepTag(Params.Protocol, RepTag_NetTest_FilterOut, RepTagInfo))
		{
			// Warning: Using the internal state requires the proper NetSerializer and dequantization to get the value.
			ObjectToFilterOutOffset.Add(ObjectIndex, RepTagInfo.InternalStateAbsoluteOffset);
		}
		else
		{
			ensureAlwaysMsgf(false, TEXT("Could not find FilterOut reptag in object index %u"), ObjectIndex);
		}
	}

	return bResult;
}

void UMockNetObjectFilterUsingFragmentData::RemoveObject(uint32 ObjectIndex, const FNetObjectFilteringInfo& Params)
{
	Super::RemoveObject(ObjectIndex, Params);

	ObjectToFilterOut.Remove(ObjectIndex);
	ObjectToFilterOutOffset.Remove(ObjectIndex);
}

void UMockNetObjectFilterUsingFragmentData::UpdateObjects(FNetObjectFilterUpdateParams& Params)
{
	++CallStatus.CallCounts.UpdateObjects;

	bool bIsProperCall = Params.ObjectCount <= AddedCount;
	for (uint32 ObjectIndex : MakeArrayView(Params.ObjectIndices, Params.ObjectCount))
	{
		bIsProperCall = bIsProperCall && AddedObjectIndices.GetBit(ObjectIndex);

		if (UPTRINT* InternalStateOffset = ObjectToFilterOutOffset.Find(ObjectIndex))
		{
			// Warning: This is generally not safe, not even for primitive types.
			const bool bFilterOut = *reinterpret_cast<const bool*>((*Params.StateBuffers)[ObjectIndex] + *InternalStateOffset);
			ObjectToFilterOut.Emplace(ObjectIndex, bFilterOut);
		}
	}
	CallStatus.SuccessfulCallCounts.UpdateObjects += bIsProperCall;
}

void UMockNetObjectFilterUsingFragmentData::Filter(FNetObjectFilteringParams& Params)
{
	Super::Filter(Params);

	// Go through objects with FilterOut wishes
	for (const auto& Pair : ObjectToFilterOut)
	{
		Params.OutAllowedObjects.SetBitValue(Pair.Key, !Pair.Value);
	}
}


//**************************************************************************************************
// UMockNetObjectFilterWithCondition
//**************************************************************************************************

void UMockNetObjectFilterWithCondition::OnInit(FNetObjectFilterInitParams& Params)
{
	Super::OnInit(Params);

	SetupFilterType(ENetFilterType::PrePoll_Raw);

	ReplicationSystem = Params.ReplicationSystem.Get();
}

bool UMockNetObjectFilterWithCondition::AddObject(uint32 ObjectIndex, FNetObjectFilterAddObjectParams& Params)
{
	bool bResult = Super::AddObject(ObjectIndex, Params);

	// Make sure the object is of the proper class
	UObject* ReplicatedObject = ReplicationSystem->GetReplicationSystemInternal()->GetNetRefHandleManager().GetReplicatedObjectInstance(ObjectIndex);
	UTestFilteringObject* FilterObject = CastChecked<UTestFilteringObject>(ReplicatedObject);

	return bResult;
}

void UMockNetObjectFilterWithCondition::Filter(FNetObjectFilteringParams& Params)
{
	++CallStatus.CallCounts.Filter;

	bool bIsProperCall = true;

	Params.FilteredObjects.ForAllSetBits([&](uint32 ObjectIndex)
	{
		UTestFilteringObject* FilterObject = CastChecked<UTestFilteringObject>(ReplicationSystem->GetReplicationSystemInternal()->GetNetRefHandleManager().GetReplicatedObjectInstance(ObjectIndex));
		
		const bool bIsFilteredOut = FilterObject->GetFilterOut();

		Params.OutAllowedObjects.SetBitValue(ObjectIndex, !bIsFilteredOut);
	});
	
	CallStatus.SuccessfulCallCounts.Filter += bIsProperCall;
}
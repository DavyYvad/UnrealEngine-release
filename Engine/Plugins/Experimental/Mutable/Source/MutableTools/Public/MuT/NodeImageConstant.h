// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "MuR/Image.h"
#include "MuR/Ptr.h"
#include "MuR/RefCounted.h"
#include "MuT/Node.h"
#include "MuT/NodeImage.h"


namespace mu
{

	// Forward definitions
    class NodeImageConstant;
    typedef Ptr<NodeImageConstant> NodeImageConstantPtr;
    typedef Ptr<const NodeImageConstant> NodeImageConstantPtrConst;

	class InputArchive;
	class OutputArchive;


	/** Node that outputs a constant image.
	* This node also supports "image references".
	*/
	class MUTABLETOOLS_API NodeImageConstant : public NodeImage
	{
	public:

		//-----------------------------------------------------------------------------------------
		// Life cycle
		//-----------------------------------------------------------------------------------------

		NodeImageConstant();

		void SerialiseWrapper(OutputArchive& arch) const override;
		static void Serialise( const NodeImageConstant* pNode, OutputArchive& arch );
		static NodeImageConstantPtr StaticUnserialise( InputArchive& arch );

		//-----------------------------------------------------------------------------------------
		// Node Interface
		//-----------------------------------------------------------------------------------------

        const FNodeType* GetType() const override;
		static const FNodeType* GetStaticType();

		//-----------------------------------------------------------------------------------------
		// Own Interface
		//-----------------------------------------------------------------------------------------

		//! Get the image that will be output by this node
        Ptr<const Image> GetValue() const;

        //! Set the image to be output by this node
        void SetValue( const Image* );

        //! Set the image proxy that will provide the image for this node when necessary
        void SetValue( Ptr<ResourceProxy<Image>> pImage );

		//-----------------------------------------------------------------------------------------
		// Interface pattern
		//-----------------------------------------------------------------------------------------
		class Private;
		Private* GetPrivate() const;
        Node::Private* GetBasePrivate() const override;

	protected:

		//! Forbidden. Manage with the Ptr<> template.
		~NodeImageConstant();

	private:

		Private* m_pD;

	};

}

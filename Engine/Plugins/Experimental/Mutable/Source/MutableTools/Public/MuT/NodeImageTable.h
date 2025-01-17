// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "MuR/Ptr.h"
#include "MuR/RefCounted.h"
#include "MuT/Node.h"
#include "MuT/NodeImage.h"


namespace mu
{

	// Forward definitions
	class NodeImageTable;
	typedef Ptr<NodeImageTable> NodeImageTablePtr;
	typedef Ptr<const NodeImageTable> NodeImageTablePtrConst;

	class Table;
	typedef Ptr<Table> TablePtr;
	typedef Ptr<const Table> TablePtrConst;

	class InputArchive;
	class OutputArchive;
	struct FImageDesc;


	//! This node provides the meshes stored in the column of a table.
	//! \ingroup transform
	class MUTABLETOOLS_API NodeImageTable : public NodeImage
	{
	public:

		//-----------------------------------------------------------------------------------------
		// Life cycle
		//-----------------------------------------------------------------------------------------

		NodeImageTable();

		void SerialiseWrapper(OutputArchive& arch) const override;
		static void Serialise( const NodeImageTable* pNode, OutputArchive& arch );
		static NodeImageTablePtr StaticUnserialise( InputArchive& arch );


		//-----------------------------------------------------------------------------------------
		// Node Interface
		//-----------------------------------------------------------------------------------------

        const FNodeType* GetType() const override;
		static const FNodeType* GetStaticType();

		//-----------------------------------------------------------------------------------------
		// Own Interface
		//-----------------------------------------------------------------------------------------

		//! Set the name of the implicit table parameter.
		void SetParameterName( const FString& strName );

		//!
		TablePtr GetTable() const;
		void SetTable( TablePtr );

		//!
		void SetColumn( const FString& strName );

		//!
		void SetMaxTextureSize(uint16 Size);
		uint16 GetMaxTextureSize();

		//! Adds the "None" option to the parameter that represents this table column
		void SetNoneOption(bool bAddOption);
		
		//!
		void SetReferenceImageDescriptor(const FImageDesc& Descriptor);

		//! Set the row name to be used as default value
		void SetDefaultRowName(const FString& RowName);

		//-----------------------------------------------------------------------------------------
		// Interface pattern
		//-----------------------------------------------------------------------------------------
		class Private;
		Private* GetPrivate() const;
        Node::Private* GetBasePrivate() const override;


	protected:

		//! Forbidden. Manage with the Ptr<> template.
		~NodeImageTable();

	private:

		Private* m_pD;

	};


}

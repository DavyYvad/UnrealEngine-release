// Copyright Epic Games, Inc. All Rights Reserved.

#include "Dataflow/DataflowSCommentNode.h"

#include "EdGraphNode_Comment.h"
#include "Dataflow/DataflowEdNode.h"
#include "Dataflow/DataflowNodeFactory.h"
#include "Dataflow/DataflowObject.h"
#include "Dataflow/DataflowCore.h"
#include "Logging/LogMacros.h"
#include "Widgets/Layout/SBorder.h"
#include "Settings/EditorStyleSettings.h"
#include "ScopedTransaction.h"

#define LOCTEXT_NAMESPACE "SDataflowEdNodeComment"

constexpr const char* CopyPostfix = "_copy";

//
// Add a menu option to create a graph node.
//
TSharedPtr<FAssetSchemaAction_Dataflow_CreateCommentNode_DataflowEdNode> FAssetSchemaAction_Dataflow_CreateCommentNode_DataflowEdNode::CreateAction(UEdGraph* ParentGraph, const TSharedPtr<SGraphEditor>& GraphEditor)
{
	return MakeShared<FAssetSchemaAction_Dataflow_CreateCommentNode_DataflowEdNode>(GraphEditor);
}

//
//  Created comment node
//
UEdGraphNode* FAssetSchemaAction_Dataflow_CreateCommentNode_DataflowEdNode::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	UEdGraphNode_Comment* CommentTemplate = NewObject<UEdGraphNode_Comment>(ParentGraph, FName("Comment"));

	FVector2D SpawnLocation = Location;
	FSlateRect Bounds;

	if (GraphEditor->GetBoundsForSelectedNodes(Bounds, 50.0f))
	{
		CommentTemplate->SetBounds(Bounds);
		SpawnLocation.X = CommentTemplate->NodePosX;
		SpawnLocation.Y = CommentTemplate->NodePosY;
	}

	CommentTemplate->bCommentBubbleVisible_InDetailsPanel = false;
	CommentTemplate->bCommentBubbleVisible = false;
	CommentTemplate->bCommentBubblePinned = false;

	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "DataflowEditorNewNode", "Dataflow Editor: New Comment Node"));
	ParentGraph->Modify();

	CommentTemplate->SetFlags(RF_Transactional);

	// set outer to be the graph so it doesn't go away
	CommentTemplate->Rename(NULL, ParentGraph, REN_NonTransactional);
	ParentGraph->AddNode(CommentTemplate, true, bSelectNewNode);

	CommentTemplate->CreateNewGuid();
	CommentTemplate->PostPlacedNewNode();
	CommentTemplate->AllocateDefaultPins();
	CommentTemplate->AutowireNewNode(FromPin);

	CommentTemplate->NodePosX = SpawnLocation.X;
	CommentTemplate->NodePosY = SpawnLocation.Y;
	CommentTemplate->SnapToGrid(GetDefault<UEditorStyleSettings>()->GridSnapSize);

	ParentGraph->NotifyGraphChanged();

	return CommentTemplate;
}

//void FAssetSchemaAction_Dataflow_CreateCommentNode_DataflowEdNode::AddReferencedObjects(FReferenceCollector& Collector)
//{
//	FEdGraphSchemaAction::AddReferencedObjects(Collector);
//	Collector.AddReferencedObject(NodeTemplate);
//}

TSharedPtr<FAssetSchemaAction_Dataflow_DuplicateCommentNode_DataflowEdNode> FAssetSchemaAction_Dataflow_DuplicateCommentNode_DataflowEdNode::CreateAction(UEdGraph* ParentGraph, const TSharedPtr<SGraphEditor>& GraphEditor)
{
	return MakeShared<FAssetSchemaAction_Dataflow_DuplicateCommentNode_DataflowEdNode>(GraphEditor);
}

//
//  Duplicate comment node
//
UEdGraphNode* FAssetSchemaAction_Dataflow_DuplicateCommentNode_DataflowEdNode::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	FString NewNodeName = CommentNodeToDuplicate->NodeComment;

	UEdGraphNode_Comment* CommentTemplate = NewObject<UEdGraphNode_Comment>(ParentGraph, FName(*NewNodeName));

	CommentTemplate->bCommentBubbleVisible_InDetailsPanel = false;
	CommentTemplate->bCommentBubbleVisible = false;
	CommentTemplate->bCommentBubblePinned = false;

//	const FScopedTransaction Transaction(NSLOCTEXT("UnrealEd", "DataflowEditorDuplicatedNode", "Dataflow Editor: New Comment Node (by using Duplicate)"));

	ParentGraph->Modify();

	CommentTemplate->SetFlags(RF_Transactional);

	// set outer to be the graph so it doesn't go away
	CommentTemplate->Rename(NULL, ParentGraph, REN_NonTransactional);
	ParentGraph->AddNode(CommentTemplate, true, bSelectNewNode);

	CommentTemplate->CreateNewGuid();
	CommentTemplate->PostPlacedNewNode();
	CommentTemplate->AllocateDefaultPins();
	CommentTemplate->AutowireNewNode(FromPin);

	CommentTemplate->NodePosX = Location.X;
	CommentTemplate->NodePosY = Location.Y;
	CommentTemplate->NodeWidth = CommentNodeToDuplicate->NodeWidth;
	CommentTemplate->NodeHeight = CommentNodeToDuplicate->NodeHeight;
	CommentTemplate->SnapToGrid(GetDefault<UEditorStyleSettings>()->GridSnapSize);
	CommentTemplate->CommentColor = CommentNodeToDuplicate->CommentColor;
	CommentTemplate->FontSize = CommentNodeToDuplicate->FontSize;

	CommentTemplate->NodeComment = NewNodeName;

	ParentGraph->NotifyGraphChanged();

	return CommentTemplate;
}


void FAssetSchemaAction_Dataflow_DuplicateCommentNode_DataflowEdNode::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(CommentNodeToDuplicate);
}

TSharedPtr<FAssetSchemaAction_Dataflow_PasteCommentNode_DataflowEdNode> FAssetSchemaAction_Dataflow_PasteCommentNode_DataflowEdNode::CreateAction(UEdGraph* ParentGraph, const TSharedPtr<SGraphEditor>& GraphEditor)
{
	return MakeShared<FAssetSchemaAction_Dataflow_PasteCommentNode_DataflowEdNode>(GraphEditor);
}

//
//  Paste comment node
//
UEdGraphNode* FAssetSchemaAction_Dataflow_PasteCommentNode_DataflowEdNode::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	FString NewNodeName = NodeName.ToString();

	UEdGraphNode_Comment* CommentTemplate = NewObject<UEdGraphNode_Comment>(ParentGraph, FName(*NewNodeName));

	CommentTemplate->bCommentBubbleVisible_InDetailsPanel = false;
	CommentTemplate->bCommentBubbleVisible = false;
	CommentTemplate->bCommentBubblePinned = false;

	ParentGraph->Modify();

	CommentTemplate->SetFlags(RF_Transactional);

	// set outer to be the graph so it doesn't go away
	CommentTemplate->Rename(NULL, ParentGraph, REN_NonTransactional);
	ParentGraph->AddNode(CommentTemplate, true, bSelectNewNode);

	CommentTemplate->CreateNewGuid();
	CommentTemplate->PostPlacedNewNode();
	CommentTemplate->AllocateDefaultPins();
	CommentTemplate->AutowireNewNode(FromPin);

	CommentTemplate->NodePosX = Location.X;
	CommentTemplate->NodePosY = Location.Y;
	CommentTemplate->NodeWidth = Size.X;
	CommentTemplate->NodeHeight = Size.Y;
	CommentTemplate->SnapToGrid(GetDefault<UEditorStyleSettings>()->GridSnapSize);
	CommentTemplate->CommentColor = Color;
	CommentTemplate->FontSize = FontSize;

	CommentTemplate->NodeComment = NewNodeName;

	ParentGraph->NotifyGraphChanged();

	return CommentTemplate;
}

#undef LOCTEXT_NAMESPACE

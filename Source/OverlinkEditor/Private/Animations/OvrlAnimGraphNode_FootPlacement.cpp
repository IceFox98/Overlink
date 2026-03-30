// Copyright Epic Games, Inc. All Rights Reserved.

#include "Animations/OvrlAnimGraphNode_FootPlacement.h"

/////////////////////////////////////////////////////
// UOvrlAnimGraphNode_FootPlacement

#define LOCTEXT_NAMESPACE "AnimGraphNode_FootPlacement"

UOvrlAnimGraphNode_FootPlacement::UOvrlAnimGraphNode_FootPlacement()
{
}

UOvrlAnimGraphNode_FootPlacement::UOvrlAnimGraphNode_FootPlacement(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

FText UOvrlAnimGraphNode_FootPlacement::GetControllerDescription() const
{
	return LOCTEXT("FootPlacement", "Ovrl Foot Placement");
}

FText UOvrlAnimGraphNode_FootPlacement::GetTooltipText() const
{
	return LOCTEXT("FootPlacementTooltip", "Ovrl Foot Placement.");
}

FLinearColor UOvrlAnimGraphNode_FootPlacement::GetNodeTitleColor() const
{
	return FLinearColor(FColor(153.f, 0.f, 0.f));
}

FText UOvrlAnimGraphNode_FootPlacement::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return GetControllerDescription();
}

#undef LOCTEXT_NAMESPACE

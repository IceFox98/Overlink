// Copyright Epic Games, Inc. All Rights Reserved.

#include "Animations/OvrlAnimGraphNode_Steering.h"
#include "Animation/AnimRootMotionProvider.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/CompilerResultsLog.h"
#include "DetailCategoryBuilder.h"
#include "DetailLayoutBuilder.h"
#include "PropertyHandle.h"
#include "ScopedTransaction.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OvrlAnimGraphNode_Steering)

#define LOCTEXT_NAMESPACE "AnimationWarping"

UOvrlAnimGraphNode_Steering::UOvrlAnimGraphNode_Steering(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

FText UOvrlAnimGraphNode_Steering::GetControllerDescription() const
{
	return LOCTEXT("Steering", "Ovrl Steering");
}

FText UOvrlAnimGraphNode_Steering::GetTooltipText() const
{
	return LOCTEXT("SteeringTooltip", "Rotates the root and lower body by the specified angle, while counter rotating the upper body to maintain the forward facing direction.");
}

FText UOvrlAnimGraphNode_Steering::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return GetControllerDescription();
}

FLinearColor UOvrlAnimGraphNode_Steering::GetNodeTitleColor() const
{
	return FLinearColor(FColor(153.f, 0.f, 0.f));
}

void UOvrlAnimGraphNode_Steering::CustomizePinData(UEdGraphPin* Pin, FName SourcePropertyName, int32 ArrayIndex) const
{
	Super::CustomizePinData(Pin, SourcePropertyName, ArrayIndex);
}

void UOvrlAnimGraphNode_Steering::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_FUNC()
	Super::CustomizeDetails(DetailBuilder);
}

void UOvrlAnimGraphNode_Steering::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_FUNC()
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UOvrlAnimGraphNode_Steering::GetInputLinkAttributes(FNodeAttributeArray& OutAttributes) const
{
	OutAttributes.Add(UE::Anim::IAnimRootMotionProvider::AttributeName);
}

void UOvrlAnimGraphNode_Steering::GetOutputLinkAttributes(FNodeAttributeArray& OutAttributes) const
{
	OutAttributes.Add(UE::Anim::IAnimRootMotionProvider::AttributeName);
}

void UOvrlAnimGraphNode_Steering::ValidateAnimNodeDuringCompilation(USkeleton* ForSkeleton, FCompilerResultsLog& MessageLog)
{
	Super::ValidateAnimNodeDuringCompilation(ForSkeleton, MessageLog);
}

#undef LOCTEXT_NAMESPACE

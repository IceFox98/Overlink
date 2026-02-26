// Copyright Epic Games, Inc. All Rights Reserved.

#include "Animations/OvrlAnimGraphNode_OffsetRootBone.h"
#include "Animation/AnimRootMotionProvider.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "DetailLayoutBuilder.h"
#include "PropertyHandle.h"
#include "ScopedTransaction.h"

#define LOCTEXT_NAMESPACE "AnimationWarping"

UOvrlAnimGraphNode_OffsetRootBone::UOvrlAnimGraphNode_OffsetRootBone(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

FText UOvrlAnimGraphNode_OffsetRootBone::GetTooltipText() const
{
	return LOCTEXT("OffsetRootBoneTooltip", "Applies an offset to the root bone");
}

FText UOvrlAnimGraphNode_OffsetRootBone::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("OvrlOffsetRootBone", "Ovrl Offset Root Bone");
}

FLinearColor UOvrlAnimGraphNode_OffsetRootBone::GetNodeTitleColor() const
{
	return FLinearColor(FColor(153.f, 0.f, 0.f));
}

void UOvrlAnimGraphNode_OffsetRootBone::CustomizePinData(UEdGraphPin* Pin, FName SourcePropertyName, int32 ArrayIndex) const
{
	Super::CustomizePinData(Pin, SourcePropertyName, ArrayIndex);

	if (Pin->PinName == GET_MEMBER_NAME_STRING_CHECKED(FOvrlAnimNode_OffsetRootBone, TranslationDelta))
	{
		Pin->bHidden = (Node.EvaluationMode == EWarpingEvaluationMode::Graph);
	}

	if (Pin->PinName == GET_MEMBER_NAME_STRING_CHECKED(FOvrlAnimNode_OffsetRootBone, RotationDelta))
	{
		Pin->bHidden = (Node.EvaluationMode == EWarpingEvaluationMode::Graph);
	}
}

void UOvrlAnimGraphNode_OffsetRootBone::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_FUNC()
	Super::CustomizeDetails(DetailBuilder);

	TSharedRef<IPropertyHandle> NodeHandle = DetailBuilder.GetProperty(FName(TEXT("Node")), GetClass());

	if (Node.EvaluationMode == EWarpingEvaluationMode::Graph)
	{
		DetailBuilder.HideProperty(NodeHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FOvrlAnimNode_OffsetRootBone, TranslationDelta)));
		DetailBuilder.HideProperty(NodeHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FOvrlAnimNode_OffsetRootBone, RotationDelta)));
	}
}

void UOvrlAnimGraphNode_OffsetRootBone::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	DECLARE_SCOPE_HIERARCHICAL_COUNTER_FUNC()
	Super::PostEditChangeProperty(PropertyChangedEvent);

	bool bRequiresNodeReconstruct = false;
	FProperty* ChangedProperty = PropertyChangedEvent.Property;

	if (ChangedProperty)
	{
		if ((ChangedProperty->GetFName() == GET_MEMBER_NAME_STRING_CHECKED(FOvrlAnimNode_OffsetRootBone, EvaluationMode)))
		{
			FScopedTransaction Transaction(LOCTEXT("OffsetRootBone_ChangeEvaluationMode", "Offset Root Bone Change Evaluation Mode"));
			Modify();

			// Break links to pins going away
			for (int32 PinIndex = 0; PinIndex < Pins.Num(); ++PinIndex)
			{
				UEdGraphPin* Pin = Pins[PinIndex];
				if (Pin->PinName == GET_MEMBER_NAME_STRING_CHECKED(FOvrlAnimNode_OffsetRootBone, TranslationDelta))
				{
					if (Node.EvaluationMode == EWarpingEvaluationMode::Graph)
					{
						Pin->BreakAllPinLinks();
					}
				}
				else if (Pin->PinName == GET_MEMBER_NAME_STRING_CHECKED(FOvrlAnimNode_OffsetRootBone, RotationDelta))
				{
					if (Node.EvaluationMode == EWarpingEvaluationMode::Graph)
					{
						Pin->BreakAllPinLinks();
					}
				}
			}

			bRequiresNodeReconstruct = true;
		}
	}

	if (bRequiresNodeReconstruct)
	{
		ReconstructNode();
		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
	}
}

void UOvrlAnimGraphNode_OffsetRootBone::GetInputLinkAttributes(FNodeAttributeArray& OutAttributes) const
{
	OutAttributes.Add(UE::Anim::IAnimRootMotionProvider::AttributeName);
}

void UOvrlAnimGraphNode_OffsetRootBone::GetOutputLinkAttributes(FNodeAttributeArray& OutAttributes) const
{
	OutAttributes.Add(UE::Anim::IAnimRootMotionProvider::AttributeName);
}

#undef LOCTEXT_NAMESPACE

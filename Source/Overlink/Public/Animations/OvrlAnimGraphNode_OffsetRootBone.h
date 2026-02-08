// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AnimGraphNode_Base.h"
#include "Animations/OvrlAnimNode_OffsetRootBone.h"

#include "OvrlAnimGraphNode_OffsetRootBone.generated.h"

namespace ENodeTitleType { enum Type : int; }

UCLASS(Experimental)
class UOvrlAnimGraphNode_OffsetRootBone : public UAnimGraphNode_Base
{
	GENERATED_UCLASS_BODY()


	UPROPERTY(EditAnywhere, Category = Settings)
	FOvrlAnimNode_OffsetRootBone Node;

public:
	// UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetTooltipText() const override;
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void GetInputLinkAttributes(FNodeAttributeArray& OutAttributes) const override;
	virtual void GetOutputLinkAttributes(FNodeAttributeArray& OutAttributes) const override;
	// End of UEdGraphNode interface

protected:
	// UAnimGraphNode_Base interface
	virtual void CustomizePinData(UEdGraphPin* Pin, FName SourcePropertyName, int32 ArrayIndex) const override;
	// End of UAnimGraphNode_Base interface

};

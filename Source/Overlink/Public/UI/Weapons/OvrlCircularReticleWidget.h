// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/Widget.h"
#include "UI/Weapons/SCircularReticleWidget.h"

#include "OvrlCircularReticleWidget.generated.h"

class SWidget;

/**
 *
 */
UCLASS()
class OVERLINK_API UOvrlCircularReticleWidget : public UWidget
{
	GENERATED_BODY()

public:
	UOvrlCircularReticleWidget(const FObjectInitializer& ObjectInitializer);

public:
	virtual void SynchronizeProperties() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

public:

	void SetRadius(float NewRadius);
	void SetOpacity(float NewOpacity);

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Circular Reticle Widget")
	TArray<FCircularReticleSpokeData> ReticleSpokes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Circular Reticle Widget")
	FSlateBrush ReticleSpokeImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Circular Reticle Widget", meta = (ClampMin = 0.0))
	float Radius;

private:
	// Internal Slate widget representing the actual reticle visuals
	TSharedPtr<SCircularReticleWidget> SlateReticleWidget;
};

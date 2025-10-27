// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Styling/CoreStyle.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SLeafWidget.h"

#include "SCircularReticleWidget.generated.h"

USTRUCT(BlueprintType)
struct FCircularReticleSpokeData
{
	GENERATED_BODY()

public:

	// The angle of the circular reticle where the spoke should be placed on
	UPROPERTY(EditAnywhere, meta = (ForceUnits = deg))
		float PositionAngle;

	// The local rotation of the spoke image
	UPROPERTY(EditAnywhere, meta = (ForceUnits = deg))
		float ImageRotationAngle;
};

class SCircularReticleWidget : public SLeafWidget
{
	SLATE_BEGIN_ARGS(SCircularReticleWidget)
		: _ReticleSpokeBrush(FCoreStyle::Get().GetBrush("Throbber.CircleChunk"))
		, _Radius(48.0f)
	{
	}
	// What each reticle spoke on the circumference looks like
	SLATE_ARGUMENT(const FSlateBrush*, ReticleSpokeBrush)
		// At which angles should a spoke be drawn
		SLATE_ARGUMENT(TArray<FCircularReticleSpokeData>, SpokeList)
		// The radius of the circle
		SLATE_ATTRIBUTE(float, Radius)
		// The color and opacity of the spoke
		SLATE_ATTRIBUTE(FSlateColor, ColorAndOpacity)
		SLATE_END_ARGS()


public:
	SCircularReticleWidget();

	void Construct(const FArguments& InArgs);

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FVector2D ComputeDesiredSize(float) const override;
	virtual bool ComputeVolatility() const override { return true; }

	void SetRadius(float NewRadius);
	void SetSpokeList(TArray<FCircularReticleSpokeData>& NewSpokeList);

private:
	// What each spoke on the circumference looks like
	const FSlateBrush* ReticleSpokeBrush;

	// Angles around the reticle center to place ReticleCornerImage icons
	TArray<FCircularReticleSpokeData> SpokeList;

	// The radius of the circle
	TAttribute<float> Radius;

	// Color and opacity of the spokes
	TAttribute<FSlateColor> ColorAndOpacity;
	bool bColorAndOpacitySet;
};

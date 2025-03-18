// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Weapons/SCircularReticleWidget.h"

SCircularReticleWidget::SCircularReticleWidget()
{
}

void SCircularReticleWidget::Construct(const FArguments& InArgs)
{
	ReticleSpokeBrush = InArgs._ReticleSpokeBrush;
	SpokeList = InArgs._SpokeList;
	Radius = InArgs._Radius;
	bColorAndOpacitySet = InArgs._ColorAndOpacity.IsSet();
	ColorAndOpacity = InArgs._ColorAndOpacity;
}

int32 SCircularReticleWidget::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const bool bIsEnabled = ShouldBeEnabled(bParentEnabled);
	const ESlateDrawEffect DrawEffects = bIsEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;
	const FVector2D LocalCenter = AllottedGeometry.GetLocalPositionAtCoordinates(FVector2D(0.5f, 0.5f));

	const bool bShouldDrawSpokes = (SpokeList.Num() > 0) && (ReticleSpokeBrush != nullptr);

	if (bShouldDrawSpokes)
	{
		const FLinearColor SpokeColor =
			bColorAndOpacitySet ? ColorAndOpacity.Get().GetColor(InWidgetStyle) : (InWidgetStyle.GetColorAndOpacityTint() * ReticleSpokeBrush->GetTint(InWidgetStyle));

		for (const FCircularReticleSpokeData& SpokeData : SpokeList)
		{
			const FQuat2D SpokeLocalRotation = FQuat2D(FMath::DegreesToRadians(SpokeData.ImageRotationAngle));

			// First rotate the corner image about the origin (top left)
			// 1. Translate the center so that it matches the origin
			// 2. Apply the rotation (SpokeLocalRotation)
			// 3. Translate back to the original position
			FSlateRenderTransform RotateAboutOrigin = Concatenate(
				FVector2D(-ReticleSpokeBrush->ImageSize.X * 0.5f, -ReticleSpokeBrush->ImageSize.Y * 0.5f),
				SpokeLocalRotation,
				FVector2D(ReticleSpokeBrush->ImageSize.X * 0.5f, ReticleSpokeBrush->ImageSize.Y * 0.5f));

			// Since its a circular reticle, we can calculate the 2D coords using trigonomerty
			const FVector2D SpokeLocation = Radius.Get() * FVector2D(FMath::Sin(FMath::DegreesToRadians(SpokeData.PositionAngle)), -FMath::Cos(FMath::DegreesToRadians(SpokeData.PositionAngle)));

			const FSlateRenderTransform SpokeTransform = Concatenate(RotateAboutOrigin, SpokeLocation);

			// Paint the spoke
			const FPaintGeometry Geometry = AllottedGeometry.ToPaintGeometry(ReticleSpokeBrush->ImageSize, FSlateLayoutTransform(LocalCenter - (ReticleSpokeBrush->ImageSize * 0.5f)), SpokeTransform, FVector2D(0.0f, 0.0f));
			FSlateDrawElement::MakeBox(OutDrawElements, LayerId, Geometry, ReticleSpokeBrush, DrawEffects, SpokeColor);
		}
	}

	return LayerId;
}

FVector2D SCircularReticleWidget::ComputeDesiredSize(float) const
{
	check(ReticleSpokeBrush);
	const float SampledRadius = Radius.Get();
	return FVector2D((ReticleSpokeBrush->ImageSize.X + SampledRadius) * 2.0f, (ReticleSpokeBrush->ImageSize.Y + SampledRadius) * 2.0f);
}

void SCircularReticleWidget::SetRadius(float NewRadius)
{
	if (Radius.IsBound() || (Radius.Get() != NewRadius))
	{
		Radius = NewRadius;
		Invalidate(EInvalidateWidgetReason::Layout); // Force redraw
	}
}

void SCircularReticleWidget::SetSpokeList(TArray<FCircularReticleSpokeData>& NewSpokeList)
{
	SpokeList = NewSpokeList;
}

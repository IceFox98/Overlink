// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Weapons/OvrlCircularReticleWidget.h"

UOvrlCircularReticleWidget::UOvrlCircularReticleWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetVisibility(ESlateVisibility::HitTestInvisible);
	bIsVolatile = true;
}

void UOvrlCircularReticleWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	SlateReticleWidget.Reset();
}

void UOvrlCircularReticleWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	SlateReticleWidget->SetRadius(Radius);
	SlateReticleWidget->SetSpokeList(ReticleSpokes);
}

TSharedRef<SWidget> UOvrlCircularReticleWidget::RebuildWidget()
{
	SlateReticleWidget = SNew(SCircularReticleWidget)
		.ReticleSpokeBrush(&ReticleSpokeImage)
		.Radius(this->Radius)
		.SpokeList(this->ReticleSpokes);

	return SlateReticleWidget.ToSharedRef();
}

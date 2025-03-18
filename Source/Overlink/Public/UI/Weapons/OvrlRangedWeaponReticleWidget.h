// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/Weapons/OvrlReticleWidgetBase.h"

#include "OvrlRangedWeaponReticleWidget.generated.h"

class UOvrlCircularReticleWidget;

/**
 *
 */
UCLASS()
class OVERLINK_API UOvrlRangedWeaponReticleWidget : public UOvrlReticleWidgetBase
{
	GENERATED_BODY()

protected:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Reticle Widget", meta = (BindWidget))
		TObjectPtr<UOvrlCircularReticleWidget> CrosshairReticle;
};

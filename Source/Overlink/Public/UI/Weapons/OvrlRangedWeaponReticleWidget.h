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

public:

	virtual void InitializeFromWeapon(AOvrlRangedWeaponInstance* Weapon) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Ovrl Ranged Weapon Reticle Widget", meta = (DisplayName = "Show Hit Marker"))
	void K2_ShowHitMarker();

protected:

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:

	void OnWeaponHitSomething(const FHitResult& HitData);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Reticle Widget", meta = (BindWidget))
		TObjectPtr<UOvrlCircularReticleWidget> CrosshairReticle;

private:


};

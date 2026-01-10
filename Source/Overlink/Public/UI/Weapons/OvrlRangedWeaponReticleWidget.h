// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UI/Weapons/OvrlReticleWidgetBase.h"

#include "OvrlRangedWeaponReticleWidget.generated.h"

class UOvrlCircularReticleWidget;
class UImage;

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

	UFUNCTION()
	void OnWeaponDestroyed(AActor* DestroyedActor);

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Ranged Weapon Reticle Widget", meta = (BindWidget))
	TObjectPtr<UOvrlCircularReticleWidget> CrosshairReticle;

	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Ranged Weapon Reticle Widget", meta = (BindWidgetOptional))
	TObjectPtr<UImage> CenterDot;

	// Multiplier applied to the widget fading time when weapon switches from no-ADS to ADS.
	// Higher values means more time to fade out.
	UPROPERTY(EditAnywhere, Category = "Ovrl Ranged Weapon Reticle Widget")
	float FadeOutTimeMultiplier = 2.f;

	// Multiplier applied to the widget fading time when weapon switches from ADS to no-ADS.
	// Higher values means more time to fade in.
	UPROPERTY(EditAnywhere, Category = "Ovrl Ranged Weapon Reticle Widget")
	float FadeInTimeMultiplier = 2.f;

private:

	float FadeOutTime = 0.f;
	float CurrentAlpha = 1.f;
};

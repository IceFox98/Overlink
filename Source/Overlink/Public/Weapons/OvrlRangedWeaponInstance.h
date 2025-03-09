// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/OvrlWeaponInstance.h"

#include "OvrlRangedWeaponInstance.generated.h"

/**
 *
 */
UCLASS()
class OVERLINK_API AOvrlRangedWeaponInstance : public AOvrlWeaponInstance
{
	GENERATED_BODY()

public:

	AOvrlRangedWeaponInstance();

public:

	void AddSpread();

	FORCEINLINE float GetTimeBetweenShots() const { return 60.f / FireRate; };

	UFUNCTION(BlueprintCallable, Category = "Ovrl|Ranged Weapon Instance")
		FTransform GetMuzzleTransform() const;

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl|Ranged Weapon Instance")
		FName MuzzleSocketName;

	// Number of bullets to fire in a single cartridge (typically 1, but may be more for shotguns)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl|Ranged Weapon Instance")
		int32 BulletsPerCartridge;

	// The maximum distance at which this weapon can deal damage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl|Ranged Weapon Instance", meta = (ForceUnits = cm))
		float MaxDamageRange;

	/** The time (in seconds) you need to reload the weapon */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl|Ranged Weapon Instance")
		float ReloadingTime;

	/** The fire rate of this weapon. This will represent the amount of bullets shot per minute */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl|Ranged Weapon Instance", meta = (ClampMin = 0.0f))
		float FireRate;
};

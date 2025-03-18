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

	virtual void Tick(float DeltaTime) override;

	virtual void OnEquipped() override;

public:

	virtual void Fire(const FHitResult& HitData) override;

	FORCEINLINE float GetTimeBetweenShots() const { return 60.f / FireRate; };
	FORCEINLINE float GetMaxDamageRange() const { return MaxDamageRange; };
	FORCEINLINE float GetSpreadAngle() const { return CurrentSpread; };
	FORCEINLINE FTransform GetWeaponKickbackRecoil() const { return CurrentKickbackRecoil; };
	FORCEINLINE float GetWeaponCameraRecoil() const { return CurrentCameraRecoil; };

	UFUNCTION(BlueprintCallable, Category = "Ovrl Ranged Weapon Instance")
		FTransform GetMuzzleTransform() const;

protected:

	void AddSpread();

	void UpdateRecoil(float DeltaTime);
	void UpdateSpread(float DeltaTime);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance")
		FName MuzzleSocketName;

	// Number of bullets to fire in a single cartridge (typically 1, but may be more for shotguns)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance")
		int32 BulletsPerCartridge;

	// The maximum distance at which this weapon can deal damage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance", meta = (ForceUnits = cm))
		float MaxDamageRange;

	/** The time (in seconds) you need to reload the weapon */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance")
		float ReloadingTime;

	/** The fire rate of this weapon. This will represent the amount of bullets shot per minute */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance", meta = (ClampMin = 0.0f))
		float FireRate;

	// The recoil that will be applied to the weapon mesh, during the animation.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Recoil")
		FTransform KickbackRecoil;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Recoil")
		float KickbackRecoverySpeed;

	// The recoil that will be applied to the player camera (pitch).
	// This value is accumulated each shot.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Recoil")
		float CameraRecoil;

	// The maximum camera recoil (accumulated) that the camera can reach.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Recoil")
		float CameraMaxRecoil;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Recoil")
		float CameraRecoilRecoverySpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Spread")
		float MinSpreadAngle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Spread")
		float MaxSpreadAngle;

	// How much spread should be added for each shot (angle in degrees, radial)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Spread")
		float SpreadPerShot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Spread")
		float SpreadRecoverySpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Spread")
		FRuntimeFloatCurve HeatToHeatMultiplier;

private:

	double LastFireTime;

	FTransform CurrentKickbackRecoil;

	float SpreadHeat;
	float CurrentSpread;

	float CurrentCameraRecoil;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/OvrlWeaponInstance.h"

#include "OvrlRangedWeaponInstance.generated.h"

class UOvrlCharacterMovementComponent;

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

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:

	virtual void OnEquipped() override;

	virtual void Fire(const FHitResult& HitData) override;

	FORCEINLINE float GetTimeBetweenShots() const { return 60.f / FireRate; };
	FORCEINLINE float GetMaxDamageRange() const { return MaxDamageRange; };
	FORCEINLINE float GetSpreadAngle() const { return CurrentSpread; };
	FORCEINLINE FTransform GetWeaponKickbackRecoil() const { return CurrentKickbackRecoil; };
	FORCEINLINE float GetWeaponCameraRecoil() const { return CurrentCameraRecoil; };
	FORCEINLINE float GetAimSpeed() const { return AimSpeed; };

	UFUNCTION(BlueprintCallable, Category = "Ovrl Ranged Weapon Instance")
		FTransform GetMuzzleTransform() const;

	UFUNCTION(BlueprintCallable, Category = "Ovrl Ranged Weapon Instance")
		FTransform GetAimTransform() const;

	UFUNCTION(BlueprintCallable, Category = "Ovrl Ranged Weapon Instance")
		bool IsADS() const { return bIsADS; };

	UFUNCTION(BlueprintCallable, Category = "Ovrl Ranged Weapon Instance")
		void ToggleADS(bool bEnable) { bIsADS = bEnable; };

protected:

	void AddSpread();

	void UpdateRecoil(float DeltaTime);
	void UpdateSpread(float DeltaTime);
	void UpdateSpreadMultiplier(float DeltaTime);

	virtual void SpawnFireVFX(const FHitResult& HitData);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance")
		FName MuzzleSocketName;

	// Number of bullets to fire in a single cartridge (typically 1, but may be more for shotguns)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance")
		int32 BulletsPerCartridge;

	// The maximum distance at which this weapon can deal damage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance", meta = (ForceUnits = cm))
		float MaxDamageRange;

	// The time (in seconds) you need to reload the weapon
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance")
		float ReloadingTime;

	// The fire rate of this weapon. This will represent the amount of bullets shot per minute
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance", meta = (ClampMin = 0.0f))
		float FireRate;

	// How quickly the weapon enters to ADS
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance", meta = (ClampMin = 0.0f))
		float AimSpeed;

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

	UPROPERTY(VisibleAnywhere, Category = "Ovrl Ranged Weapon Instance|Spread")
		float MinSpreadAngle;

	UPROPERTY(VisibleAnywhere, Category = "Ovrl Ranged Weapon Instance|Spread")
		float MaxSpreadAngle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Spread")
		float SpreadRecoverySpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Spread")
		FRuntimeFloatCurve HeatToSpread;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Spread")
		FRuntimeFloatCurve HeatToHeatPerShot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Spread|Multipliers")
		float SpreadMultiplierWalking;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Spread|Multipliers")
		float SpreadMultiplierRunning;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Spread|Multipliers")
		float SpreadMultiplierCrouchStanding;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Spread|Multipliers")
		float SpreadMultiplierCrouchWalking;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Spread|Multipliers")
		float SpreadMultiplierFalling;

	UPROPERTY(EditDefaultsOnly, Category = "Ovrl Ranged Weapon Instance")
		UNiagaraSystem* MuzzleFlashVFX;

private:

	UPROPERTY()
		UOvrlCharacterMovementComponent* OwnerMovementComp;

	FTransform CurrentKickbackRecoil;

	float SpreadMultiplier;
	float CurrentHeat;
	float CurrentSpread;

	float CurrentCameraRecoil;

	bool bIsADS;
};

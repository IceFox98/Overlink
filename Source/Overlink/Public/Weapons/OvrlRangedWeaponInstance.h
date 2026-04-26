// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/OvrlWeaponInstance.h"

#include "OvrlRangedWeaponInstance.generated.h"

UENUM()
enum class ESightMagnification
{
	One UMETA(DisplayName = "1x"),
	OneHalf UMETA(DisplayName = "1.5x"),
	Two UMETA(DisplayName = "2x"),
	TwoHalf UMETA(DisplayName = "2.5x"),
	Three UMETA(DisplayName = "3x"),
	Four UMETA(DisplayName = "4x"),
	Five UMETA(DisplayName = "5x")
};

class UOvrlCharacterMovementComponent;
class UOvrlCameraModifierBase;
class UMaterialInstanceDynamic;
class UAnimMontage;
class UAnimSequence;
class UOvrlItemAmmoBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAmmoUpdated);

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
	virtual void OnEquipped_Implementation() override;
	virtual void OnBeforeUnequip_Implementation() override;

	virtual void Fire(const TArray<FHitResult>& HitsData) override;
	virtual void StopFire() override;

	virtual void StartReloading() override;
	virtual void EndReloading() override;

	FORCEINLINE float GetTimeBetweenShots() const { return bIsSingleShot ? 0.f : 60.f / FireRate; };
	FORCEINLINE float GetMaxDamageRange() const { return MaxDamageRange; };
	FORCEINLINE float GetSpreadAngle() const { return CurrentSpread; };
	FORCEINLINE FTransform GetWeaponKickbackRecoil() const { return CurrentKickbackRecoil; };
	FORCEINLINE float GetAimTime() const { return AimTime; };
	FORCEINLINE int32 GetBulletsPerCartridge() const { return BulletsPerCartridge; };

	UFUNCTION(BlueprintCallable, Category = "Ovrl Ranged Weapon Instance")
	FORCEINLINE bool GetCanFire() const { return bCanFire; };

	UFUNCTION(BlueprintCallable, Category = "Ovrl Ranged Weapon Instance")
	FTransform GetMuzzleTransform() const;

	UFUNCTION(BlueprintCallable, Category = "Ovrl Ranged Weapon Instance")
	FTransform GetAimTransform() const;

	UFUNCTION(BlueprintCallable, Category = "Ovrl Ranged Weapon Instance")
	bool IsADS() const { return bIsADS; };

	UFUNCTION(BlueprintCallable, Category = "Ovrl Ranged Weapon Instance")
	void ToggleADS(bool bEnable);

	// Get the amount of ammo the magazine can store.
	UFUNCTION(BlueprintCallable, Category = "Ovrl Ranged Weapon Instance")
	int32 GetMagazineSize() const;

	// Get the amount of ammo in the current magazine.
	UFUNCTION(BlueprintCallable, Category = "Ovrl Ranged Weapon Instance")
	int32 GetMagazineAmmo() const;

protected:
	void AddSpread();
	void AddRecoil();

	void UpdateRecoil(float DeltaTime);
	void UpdateSpread(float DeltaTime);
	void UpdateSpreadOffset(float DeltaTime);

	float GetMagnifiedFOV(float InFOV) const;

	virtual void PlayWeaponAnimation(UAnimSequence* AnimToPlay);

	UMaterialInstanceDynamic* GetMagazineAmmoCountMaterial();
	void UpdateMagazineAmmoCountDisplay();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MagazineAmmoCountDisplay;

public:
	UPROPERTY(BlueprintAssignable)
	FOnAmmoUpdated OnAmmoUpdated;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance")
	FName MuzzleSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance")
	FName AimSocketName;

	// Number of bullets to fire in a single cartridge (typically 1, but may be more for shotguns)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance")
	int32 BulletsPerCartridge;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance")
	TSubclassOf<UOvrlItemAmmoBase> AmmoType;

	// The maximum distance at which this weapon can deal damage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance", meta = (ForceUnits = cm))
	float MaxDamageRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance")
	TSoftObjectPtr<UAnimMontage> PlayerReloadMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance")
	TObjectPtr<UAnimSequence> ReloadAnimation;

	// If true, the fixed FireRate will be ignored and instead depend on how fast the player can shoot.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance")
	bool bIsSingleShot;

	// The fire rate of this weapon. This will represent the amount of bullets shot per minute
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance", meta = (ClampMin = 0.0f, EditCondition="!bIsSingleShot"))
	float FireRate;

	UPROPERTY(EditDefaultsOnly, Category = "Ovrl Ranged Weapon Instance")
	TObjectPtr<UAnimSequence> FireAnimation;

	// How much time (in seconds) the weapon needs to aim
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Aim", meta = (ClampMin = 0.0f, Units="s"))
	float AimTime;

	UPROPERTY(EditAnywhere, Category = "Ovrl Ranged Weapon Instance")
	ESightMagnification SightMagnification;
	
	// If true, the weapon can start recovering the recoil while still firing
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Recoil")
	bool bCanRecoverRecoilWhileFiring;

	// The recoil that will be applied to the weapon mesh, during the animation.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Recoil")
	FTransform KickbackRecoil;

	// How fast the weapon kickback recovers.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Recoil")
	float KickbackRecoverySpeed;

	// The recoil that will be applied to the player camera (pitch).
	// This value is accumulated each shot.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Recoil")
	float CameraRecoil;

	// The maximum camera recoil (accumulated) that the camera can reach.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Recoil")
	float CameraMaxRecoil;

	// How fast the camera recoil recovers.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Recoil")
	float CameraRecoilRecoverySpeed;

	// The minimum spread angle (in degrees) of the weapon when no offsets are applied.
	UPROPERTY(VisibleAnywhere, Category = "Ovrl Ranged Weapon Instance|Spread")
	float MinSpreadAngle;

	// The maximum spread angle (in degrees) of the weapon when no offsets are applied.
	UPROPERTY(VisibleAnywhere, Category = "Ovrl Ranged Weapon Instance|Spread")
	float MaxSpreadAngle;
	
	// The angle of the spread while the weapon is ADS.
	// Set to 0 to have maximum ADS precision.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Spread")
	float SpreadAngleADS;

	// How fast the bullet spread recovers.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Spread")
	float SpreadRecoverySpeed;

	// The heat is accumulated each shot fired, depending on the value of this curve.
	// X-axis: Current heat.
	// Y-axis: Amount of heat that will be added to the current heat.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Spread")
	FRuntimeFloatCurve HeatToHeatPerShot;

	// Converts the current heat (normalized to 0-1 range) to the weapon spread.
	// X-axis: Current heat.
	// Y-axis: Weapon spread.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Spread")
	FRuntimeFloatCurve HeatToSpread;

	// Offset spread angle that will be added to the current spread, when player is walking.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Spread|Offsets")
	float SpreadOffsetWalking;

	// Offset spread angle that will be added to the current spread, when player is running.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Spread|Offsets")
	float SpreadOffsetRunning;

	// Offset spread angle that will be added to the current spread, when player is crouching.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Spread|Offsets")
	float SpreadOffsetCrouchIdle;

	// Offset spread angle that will be added to the current spread, when player is walking while crouching.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Spread|Offsets")
	float SpreadOffsetCrouchWalking;

	// Offset spread angle that will be added to the current spread, when player is in air.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Instance|Spread|Offsets")
	float SpreadOffsetFalling;

private:
	UPROPERTY()
	TObjectPtr<UOvrlCharacterMovementComponent> OwnerMovementComp;

	UPROPERTY()
	TObjectPtr<UOvrlCameraModifierBase> CameraFOV;
	
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MagazineAmmoCountDisplayMat;

	// Spread
	float SpreadOffset;
	float CurrentHeat;
	float CurrentSpread;

	// Recoil
	FTransform CurrentKickbackRecoil;
	FRotator CurrentCameraRecoil;
	FRotator LastControllerRotation;
	FRotator DeltaRotation;
	FRotator RecoilStep;
	bool bCanRecoverFromRecoil;

	bool bIsADS;
	bool bCanFire;
};

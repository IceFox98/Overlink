// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animations/OvrlEquipmentAnimInstance.h"

#include "OvrlRangedWeaponAnimInstance.generated.h"

class AOvrlRangedWeaponInstance;

/**
 *
 */
UCLASS()
class OVERLINK_API UOvrlRangedWeaponAnimInstance : public UOvrlEquipmentAnimInstance
{
	GENERATED_BODY()

public:

	UOvrlRangedWeaponAnimInstance();

public:

	virtual void NativeThreadSafeUpdateAnimation(float DeltaTime) override;

	AOvrlRangedWeaponInstance* GetEquippedWeapon() const { return EquippedWeapon; };
	float GetWalkSwayAlphaADS() const { return WalkSwayAlphaADS; };

protected:

	virtual void OnNewItemEquipped(AOvrlEquipmentInstance* NewEquippedItem) override;
	virtual bool CheckCrouchLeaning() override;
	virtual float CalculateCrouchLeanSpeed() override;

private:

	void UpdateAim(float DeltaTime);

protected:

	// ------- CONFIG VARIABLES -------

	// How much of the looking sway animation should be applied during ADS
	UPROPERTY(EditAnywhere, Category = "Ovrl Ranged Weapon Anim Instance|ADS Sway Aplha")
	float LookingSwayAlphaADS;

	// How much of the movement sway animation should be applied during ADS
	UPROPERTY(EditAnywhere, Category = "Ovrl Ranged Weapon Anim Instance|ADS Sway Aplha")
	float MovementSwayAlphaADS;

	// How much of the walk sway animation should be applied during ADS
	UPROPERTY(EditAnywhere, Category = "Ovrl Ranged Weapon Anim Instance|ADS Sway Aplha")
	float WalkSwayAlphaADS;

	// How much of the jump sway animation should be applied during ADS
	UPROPERTY(EditAnywhere, Category = "Ovrl Ranged Weapon Anim Instance|ADS Sway Aplha")
	float JumpSwayAlphaADS;

	// ------- RUNTIME VALUES -------

	// Represents the alpha of the ADS animation
	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Ranged Weapon Anim Instance", Transient)
	float AimAlpha;

	// How much time (in seconds) the weapon needs to aim
	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Ranged Weapon Anim Instance", Transient)
	float AimTime;

	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Ranged Weapon Anim Instance", Transient)
	FVector WeaponRecoilTranslation;

	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Ranged Weapon Anim Instance", Transient)
	FRotator WeaponRecoilRotation;

	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Ranged Weapon Anim Instance", Transient)
	float WeaponCameraRecoil;

	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Ranged Weapon Anim Instance", Transient)
	FVector WeaponAimTranslation;

	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Ranged Weapon Anim Instance", Transient)
	FRotator WeaponAimRotation;

	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Ranged Weapon Anim Instance", Transient)
	FTransform LeftHandIKTransform;

protected:

	UPROPERTY()
	AOvrlRangedWeaponInstance* EquippedWeapon = nullptr;

	float LerpAimAlpha = 0.f;
};

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

protected:

	virtual void OnNewItemEquipped(AOvrlEquipmentInstance* NewEquippedItem) override;

private:

	void UpdateAim(float DeltaTime);

protected:

	// ------- CONFIG VARIABLES -------

	// How much of the looking sway should be applied during ADS
	UPROPERTY(EditAnywhere, Category = "Ovrl Ranged Weapon Anim Instance|ADS Sway Aplha")
	float LookingSwayAlphaADS;

	// How much of the jump sway should be applied during ADS
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
	FVector WeaponAimTranslation;

	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Ranged Weapon Anim Instance", Transient)
	FRotator WeaponAimRotation;

protected:

	UPROPERTY()
	AOvrlRangedWeaponInstance* EquippedWeapon = nullptr;

	float LerpAimAlpha = 0.f;
};

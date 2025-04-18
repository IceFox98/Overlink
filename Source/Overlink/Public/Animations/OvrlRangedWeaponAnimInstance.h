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

	virtual void NativeThreadSafeUpdateAnimation(float DeltaTime) override;

protected:

	virtual void OnNewItemEquipped(AOvrlEquipmentInstance* NewEquippedItem) override;

private:

	void UpdateAim(float DeltaTime);

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Ranged Weapon Anim Instance", Transient)
		float AimAlpha;

	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Ranged Weapon Anim Instance", Transient)
		float AimSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Ranged Weapon Anim Instance", Transient)
		FTransform WeaponRecoil;

	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Ranged Weapon Anim Instance", Transient)
		float WeaponCameraRecoil;

	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Ranged Weapon Anim Instance", Transient)
		FTransform WeaponAimTransform;

protected:

	UPROPERTY()
		AOvrlRangedWeaponInstance* EquippedWeapon = nullptr;
};

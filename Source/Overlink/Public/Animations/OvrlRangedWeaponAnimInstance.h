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

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Anim Instance", Transient)
		FTransform WeaponRecoil;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Anim Instance", Transient)
		float WeaponCameraRecoil;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Ranged Weapon Anim Instance", Transient)
		FTransform WeaponAimTransform;

protected:

	UPROPERTY()
		AOvrlRangedWeaponInstance* EquippedWeapon = nullptr;

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animations/OvrlLinkedAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"

#include "OvrlEquipmentAnimInstance.generated.h"

class UOvrlCharacterMovementComponent;
class AOvrlRangedWeaponInstance;
class AOvrlEquipmentInstance;

/**
 * 
 */
UCLASS()
class OVERLINK_API UOvrlEquipmentAnimInstance : public UOvrlLinkedAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeInitializeAnimation() override;

	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaTime) override;

	virtual void NativeThreadSafeUpdateAnimation(float DeltaTime) override;

protected:

	UFUNCTION()
		virtual void OnNewItemEquipped(AOvrlEquipmentInstance* NewEquippedItem);

private:

	void UpdateSwayLooking(float DeltaTime);
	void UpdateSwayMovement(float DeltaTime);

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance|Sway", Transient)
		FVector SwayLookingAmount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance|Sway", Transient)
		FVector SwayMovementAmount;

	TObjectPtr<UOvrlCharacterMovementComponent> CharacterMovementComponent;

	UPROPERTY()
		AOvrlEquipmentInstance* EquippedItem = nullptr;

private:
	FRotator SwayRotationAmount;
	FRotator LastPlayerCameraRotation;

	FVector SwayMovementAmountPrev;

	FVectorSpringState SpringStateMovement;
	FQuaternionSpringState SpringStateRotation;
};

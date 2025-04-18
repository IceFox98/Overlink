// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animations/OvrlLinkedAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"

#include "OvrlEquipmentAnimInstance.generated.h"

class UOvrlCharacterMovementComponent;
class AOvrlRangedWeaponInstance;
class AOvrlEquipmentInstance;
class UCurveVector;

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
	void UpdateSwayJump(float DeltaTime);
	void UpdateSwayWalk(float DeltaTime);

protected:

	// ------- CONFIG VARIABLES -------

	// The maximum sway rotation limit the equipped item can reach on both axis X and Y
	// X -> Yaw Sway
	// Y -> Pitch Sway
	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Anim Instance")
		FVector2D SwayRotationLimit;

	// The speed of the sway movement interpolation
	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Anim Instance")
		float SwayMovementSpeed;

	// The speed of the sway movement interpolation
	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Anim Instance")
		float SwayWalkSpeed;

	// ------- RUNTIME VALUES -------

	// Sway applied when moving the camera around (mouse movement)
	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance|Sway", Transient)
		FVector SwayLookingAmount;

	// Sway that follows the player movement
	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance|Sway", Transient)
		FVector SwayMovementAmount;

	// Sway applied when player is moving, it simulates a walking animation
	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance|Sway", Transient)
		FVector SwayWalkAmount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance")
		FVector SwayWalkRotationMultiplier;

	// Sway applied when player jumps, it simulates a jump animation
	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance|Sway", Transient)
		FVector SwayJumpAmount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance")
		FVector SwayJumpRotationMultiplier;

	// This curve defines the movement of this equipped item while the player is walking
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance")
		TObjectPtr<UCurveVector> JumpSwayCurve;

	TObjectPtr<UOvrlCharacterMovementComponent> CharacterMovementComponent;

	UPROPERTY()
		AOvrlEquipmentInstance* EquippedItem = nullptr;

private:
	FRotator SwayRotationAmount;
	FRotator LastPlayerCameraRotation;

	FVector SwayMovementAmountPrev;

	FVectorSpringState SpringStateMovement;
	FVectorSpringState SpringStateJump;
	FQuaternionSpringState SpringStateRotation;
};

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

	UOvrlEquipmentAnimInstance();

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

	// The maximum sway looking rotation limit the equipped item can reach on both axis X and Y
	// X -> Yaw Sway
	// Y -> Pitch Sway
	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Anim Instance|Sway Looking")
		FVector2D SwayLookingRotationLimit;

	// The speed of the sway movement interpolation
	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Anim Instance|Sway Movement")
		float SwayMovementSpeed;

	// How much the equipped item should move, when the player moves sideway
	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Anim Instance|Sway Movement")
		float SwayMovementMultiplier;

	// How much the equipped item should roll, when the player moves sideway
	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Anim Instance|Sway Movement")
		float SwayMovementRollMultiplier;

	// This curve defines the movement of the equipped item while the player is walking
	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Anim Instance|Sway Walk")
		TObjectPtr<UCurveVector> WalkSwayCurve;

	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Anim Instance|Sway Walk")
		FVector SwayWalkRotationMultiplier;

	// The speed of the walk sway interpolation
	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Anim Instance|Sway Walk")
		float SwayWalkSpeed;

	// This curve defines the movement of this equipped item while the player is walking
	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Anim Instance|Sway Jump")
		TObjectPtr<UCurveVector> JumpSwayCurve;

	// Multiplier applied when player jumps while moving right/left
	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Anim Instance|Sway Jump")
		FVector SwayJumpRotationMultiplier;

	// ------- RUNTIME VALUES -------

	// Sway applied when moving the camera around (mouse movement)
	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance|Sway", Transient)
		FVector SwayLookingAmount;

	// Sway that follows the player movement
	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance|Sway", Transient)
		FVector SwayMovementAmount;

	// Sway rotation applied when player moves
	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance|Sway", Transient)
		FRotator SwayMovementRotationAmount;

	// Sway applied when player is moving, it simulates a walking animation
	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance|Sway", Transient)
		FVector SwayWalkAmount;

	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance|Sway", Transient)
		FRotator SwayWalkRotationAmount;

	// Sway applied when player jumps, it simulates a jump animation
	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance|Sway", Transient)
		FVector SwayJumpAmount;

	// Sway rotation applied when player jumps
	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance|Sway", Transient)
		FRotator SwayJumpRotationAmount;

	UPROPERTY()
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

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

	virtual bool CheckCrouchLeaning();
	virtual float CalculateCrouchLeanSpeed();

private:

	void UpdateLookingSway(float DeltaTime);
	void UpdateMovementSway(float DeltaTime);
	void UpdateJumpSway(float DeltaTime);
	void UpdateWalkSway(float DeltaTime);

protected:

	// ------- CONFIG VARIABLES -------

	// Translation applied to the weapon when player is crouching
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance|Crouch")
	FVector CrouchTranslation;

	// Rotation applied to the weapon when player is crouching
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance|Crouch")
	FRotator CrouchRotation;

	// The maximum sway looking rotation limit the equipped item can reach on both axis X and Y
	// X -> Yaw Sway
	// Y -> Pitch Sway
	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Anim Instance|Sway Looking")
	FVector2D LookingSwayRotationLimit;

	// The speed of the sway movement interpolation
	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Anim Instance|Sway Movement")
	float MovementSwaySpeed;

	// How much the equipped item should move, when the player moves sideway
	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Anim Instance|Sway Movement")
	float MovementSwayMultiplier;

	// How much the equipped item should roll, when the player moves sideway
	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Anim Instance|Sway Movement")
	float MovementSwayRollMultiplier;

	// This curve defines the movement of the equipped item while the player is walking
	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Anim Instance|Sway Walk")
	TObjectPtr<UCurveVector> WalkSwayCurve;

	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Anim Instance|Sway Walk")
	FVector WalkSwayRotationMultiplier;

	// The speed of the walk sway interpolation
	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Anim Instance|Sway Walk")
	float WalkSwaySpeed;

	// This curve defines the movement of this equipped item while the player is walking
	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Anim Instance|Sway Jump")
	TObjectPtr<UCurveVector> JumpSwayCurve;

	// Multiplier applied when player jumps while moving right/left
	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Anim Instance|Sway Jump")
	FVector JumpSwayRotationMultiplier;

	// ------- RUNTIME VALUES -------

	// Represents the alpha of the weapon crouch leaning
	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance", Transient)
	float CrouchLeanAlpha;

	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance|Sway", Transient)
	float LookingSwayAlpha;

	// Sway applied when moving the camera around (mouse movement)
	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance|Sway", Transient)
	FVector LookingSwayTranslation;

	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance|Sway", Transient)
	float MovementSwayAlpha;

	// Sway that follows the player movement
	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance|Sway", Transient)
	FVector MovementSwayTranslation;

	// Sway rotation applied when player moves
	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance|Sway", Transient)
	FRotator MovementSwayRotation;

	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance|Sway", Transient)
	float WalkSwayAlpha;

	// Sway applied when player is moving, it simulates a walking animation
	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance|Sway", Transient)
	FVector WalkSwayTranslation;

	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance|Sway", Transient)
	FRotator WalkSwayRotation;

	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance|Sway", Transient)
	float JumpSwayAlpha;

	// Sway applied when player jumps, it simulates a jump animation
	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance|Sway", Transient)
	FVector JumpSwayTranslation;

	// Sway rotation applied when player jumps
	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance|Sway", Transient)
	FRotator JumpSwayRotation;

	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance|Camera", Transient)
	FRotator WallrunCameraTiltRotation;

	UPROPERTY()
	TObjectPtr<UOvrlCharacterMovementComponent> CharacterMovementComponent;

	UPROPERTY()
	AOvrlEquipmentInstance* EquippedItem = nullptr;

private:

	FRotator LookingSwayRotation;
	FRotator LastPlayerCameraRotation;
	FQuaternionSpringState SpringStateRotation;
	FVectorSpringState SpringStateJump;
};

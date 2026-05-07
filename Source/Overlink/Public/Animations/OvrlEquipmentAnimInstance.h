// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animations/OvrlLinkedAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameplayTagContainer.h"

#include "OvrlEquipmentAnimInstance.generated.h"

struct FQuickSlotEntry;
class UOvrlCharacterMovementComponent;
class AOvrlRangedWeaponInstance;
class AOvrlEquipmentInstance;
class UCurveVector;
class UOvrlStanceStatesAnimManager;
class UOvrlAnimManagerData;

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
	virtual void OnActiveSlotChanged(const FQuickSlotEntry& ActiveSlotEntry);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (BlueprintThreadSafe))
	void GetModifierValues(FGameplayTag ModifierTag, FVector& OutTranslation, FRotator& OutRotation);

private:
	void UpdateLookingSway(float DeltaTime);
	void UpdateJumpSway(float DeltaTime);
	void UpdateLeftHandIKAplha(float DeltaTime);

protected:
	// ------- CONFIG VARIABLES -------

	// The maximum sway looking rotation limit the equipped item can reach on both axis X and Y.
	// This is to avoid excessive translation with rapid mouse movements.
	// X -> Yaw Sway
	// Y -> Pitch Sway
	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Anim Instance|Looking Sway")
	FVector2D LookingSwayRotationLimit;

	// X -> Manages the forward/backward movement when Yaw rotation is applied
	// Y -> Manages the vertical movement when Pitch rotation is applied
	// Z -> Manages the vertical horizontal when Yaw rotation is applied
	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Anim Instance|Looking Sway")
	FVector LookingSwayMovementMultiplier;

	// X -> Roll
	// Y -> Pitch
	// Z -> Yaw
	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Anim Instance|Looking Sway")
	FVector LookingSwayRotationMultiplier;

	// This curve defines the movement of this equipped item while the player is walking
	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Anim Instance|Jump Sway")
	TObjectPtr<UCurveVector> JumpSwayCurve;

	// Multiplier applied when player jumps while moving right/left
	UPROPERTY(EditAnywhere, Category = "Ovrl Equipment Anim Instance|Jump Sway")
	FVector JumpSwayRotationMultiplier;

	UPROPERTY(EditAnywhere, Category = "Ovrl Linked Anim Instance")
	TArray<TSoftObjectPtr<UOvrlAnimManagerData>> ManagersData;

	// ------- RUNTIME VALUES -------

	// Hands

	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance", Transient)
	FVector RightHandInitialLocation;

	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance", Transient)
	FRotator RightHandInitialRotation;

	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance", Transient)
	float InitialTransformAlpha;

	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance", Transient)
	FTransform LeftHandIKTransform;

	// How much of the Left Hand IK is applied
	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance", Transient)
	float LeftHandIKAlpha;

	// Looking sway

	// How much of the looking sway is applied
	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance|Sway", Transient)
	float LookingSwayAlpha;

	// Translation sway applied when moving the camera around (mouse movement)
	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance|Sway", Transient)
	FVector LookingSwayTranslation;

	// Rotation sway applied when moving the camera around (mouse movement)
	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance|Sway", Transient)
	FRotator LookingSwayRotation;

	// Jump sway

	// How much of the jump sway is applied
	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance|Sway", Transient)
	float JumpSwayAlpha;

	// Sway applied when player jumps, it simulates a jump animation
	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance|Sway", Transient)
	FVector JumpSwayTranslation;

	// Sway rotation applied when player jumps
	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Anim Instance|Sway", Transient)
	FRotator JumpSwayRotation;

protected:
	UPROPERTY()
	TObjectPtr<UOvrlCharacterMovementComponent> CharacterMovementComponent;

	UPROPERTY()
	TObjectPtr<AOvrlEquipmentInstance> EquippedItem = nullptr;

	UPROPERTY()
	TArray<TObjectPtr<UOvrlStanceStatesAnimManager>> Managers;

private:
	FRotator SpineRotation;

	FRotator LastLookingSwayRotation;
	FRotator LastPlayerCameraRotation;
	FQuaternionSpringState SpringStateRotation;
	FVectorSpringState SpringStateJump;

	FVector LastMovementSwayTranslation;
	FVector LastWalkSwayTranslation;

	FVector LastLookingSwayTranslation;
	FVectorSpringState SprintStateLookingSway;

	float WalkSwayTime = 0.f;
};

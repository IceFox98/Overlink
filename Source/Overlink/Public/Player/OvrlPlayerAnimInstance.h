// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "OvrlGameplayTags.h"
#include "GameplayEffectTypes.h"

#include "OvrlPlayerAnimInstance.generated.h"

class AOvrlPlayerCharacter;
class UOvrlCharacterMovementComponent;
class AOvrlEquipmentInstance;
class AOvrlRangedWeaponInstance;

/**
 *
 */
UCLASS()
class OVERLINK_API UOvrlPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	UOvrlPlayerAnimInstance();
	
public:

	virtual void NativeInitializeAnimation() override;
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaTime) override;

public:
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ovrl Player Anim Instance", meta=(BlueprintThreadSafe))
	FRotator GetSlideSlopeRotation() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ovrl Player Anim Instance", meta=(BlueprintThreadSafe))
	bool IsIdle() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ovrl Player Anim Instance", meta=(BlueprintThreadSafe))
	bool ShouldTurnInPlace() const; // Used in Chooser Tables
	
	UFUNCTION(BlueprintCallable, Category = "Ovrl Player Anim Instance", meta=(BlueprintThreadSafe))
	FORCEINLINE void SetRootYawOffset(float InRootYawOffset) { RootYawOffset = InRootYawOffset; };
	
	UFUNCTION(BlueprintCallable, Category = "Ovrl Player Anim Instance")
	FRotator GetWallrunCameraTiltRotation() const; // Used in equipment ABP

	FORCEINLINE FRotator GetSpineRotation() const { return SpineRotation; };
	
protected:

	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Player Anim Instance", Transient)
	TObjectPtr<AOvrlPlayerCharacter> PlayerCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Player Anim Instance", Transient)
	TObjectPtr<UOvrlCharacterMovementComponent> CharacterMovementComponent;

	// ------ STATES ------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Player Anim Instance|States", Transient)
	FGameplayTag LocomotionMode = OvrlLocomotionModeTags::Grounded;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Player Anim Instance|States", Transient)
	FGameplayTag Stance = OvrlStanceTags::Standing;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Player Anim Instance|States", Transient)
	FGameplayTag Gait = OvrlGaitTags::Idle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Player Anim Instance|States", Transient)
	FGameplayTag OverlayMode = OvrlOverlayModeTags::Default;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Player Anim Instance|States", Transient)
	FGameplayTag LocomotionAction = FGameplayTag::EmptyTag;

	// Gameplay tags that can be mapped to blueprint variables. The variables will automatically update as the tags are added or removed.
	// These should be used instead of manually querying for the gameplay tags.
	UPROPERTY(EditDefaultsOnly, Category = "Ovrl Player Anim Instance|GameplayTags")
	FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;
	
	// ------ GENERAL ------
	
	// Used to handle different behaviors between FP and TP animations
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ovrl Player Anim Instance")
	bool bIsFirstPersonABP;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Player Anim Instance", Transient)
	bool bIsPlayerValid;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Player Anim Instance", Transient)
	FRotator PlayerRotation;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Player Anim Instance", Transient)
	FRotator SpineRotation;

	// Gravity-relative player camera pitch
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Player Anim Instance", Transient)
	float PitchAngle;
	
	// Yaw angle (in degrees) between the root facing direction and player camera facing.
	// Used to trigger turn-in-place animations (see RootYawThreshold).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Player Anim Instance|Turn-In-Place", Transient)
	float RootYawOffset;
	
	// Minimum root yaw angle required to trigger turn-in-place animations when standing.
	// X -> Angle limit when player is looking left (negative)
	// Y -> Angle limit when player is looking right (positive)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Player Anim Instance|Turn-In-Place")
	FVector2D StandingRootYawThreshold;
	
	// Minimum root yaw angle required to trigger turn-in-place animations when crouching.
	// X -> Angle limit when player is looking left (negative)
	// Y -> Angle limit when player is looking right (positive)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Player Anim Instance|Turn-In-Place")
	FVector2D CrouchingRootYawThreshold;

	// ------ MOVEMENT DATA ------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Player Anim Instance|Locomotion Data", Transient)
	bool bIsFalling;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Player Anim Instance|Locomotion Data", Transient)
	bool bIsCrouching;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Player Anim Instance|Locomotion Data", Transient)
	bool bIsMoving;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Player Anim Instance|Locomotion Data", Transient)
	bool bIsRunning;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Player Anim Instance|Locomotion Data", Transient)
	bool bIsSliding;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Player Anim Instance|Locomotion Data", Transient)
	bool bIsWallrunning;	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Player Anim Instance|Locomotion Data", Transient)
	bool bIsWallClinging;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Player Anim Instance|Locomotion Data", Transient)
	bool bHasJustLanded;

	// ------ IK ------

	// The location of where the right should be placed (World space).
	// Useful for mantling animation or similar.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Player Anim Instance|IK", Transient)
	FVector RightHandIKLocation;

	// The location of where the left should be placed (World space)
	// Useful for mantling animation or similar.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Player Anim Instance|IK", Transient)
	FVector LeftHandIKLocation;

};

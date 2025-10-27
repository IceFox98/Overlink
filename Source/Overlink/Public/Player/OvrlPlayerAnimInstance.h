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

public:

	virtual void NativeInitializeAnimation() override;

	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaTime) override;

	virtual void NativeThreadSafeUpdateAnimation(float DeltaTime) override;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Player Anim Instance", Transient)
		TObjectPtr<AOvrlPlayerCharacter> PlayerCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Player Anim Instance", Transient)
		TObjectPtr<UOvrlCharacterMovementComponent> CharacterMovementComponent;

	// ------ STATES ------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ovrl Player Anim Instance|States")
		FGameplayTag LocomotionMode = OvrlLocomotionModeTags::Grounded;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Player Anim Instance|States", Transient)
		FGameplayTag Stance = OvrlStanceTags::Standing;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Player Anim Instance|States", Transient)
		FGameplayTag Gait = OvrlGaitTags::Walking;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Player Anim Instance|States", Transient)
		FGameplayTag OverlayMode = OvrlOverlayModeTags::Default;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Player Anim Instance|States", Transient)
		FGameplayTag LocomotionAction = FGameplayTag::EmptyTag;

	// Gameplay tags that can be mapped to blueprint variables. The variables will automatically update as the tags are added or removed.
	// These should be used instead of manually querying for the gameplay tags.
	UPROPERTY(EditDefaultsOnly, Category = "Ovrl Player Anim Instance|GameplayTags")
		FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;

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

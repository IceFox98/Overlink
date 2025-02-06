// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "OBM_GameplayTags.h"

#include "OBM_PlayerAnimInstance.generated.h"

class AOBM_PlayerCharacter;

/**
 *
 */
UCLASS()
class ONEBULLETMAN_API UOBM_PlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:

	virtual void NativeInitializeAnimation() override;

	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaTime) override;

	virtual void NativeThreadSafeUpdateAnimation(float DeltaTime) override;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
		TObjectPtr<AOBM_PlayerCharacter> Character;

	//------ TAGS ------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
		FGameplayTag LocomotionMode = OBM_LocomotionModeTags::Grounded;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
		FGameplayTag Stance = OBM_StanceTags::Standing;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
		FGameplayTag Gait = OBM_GaitTags::Walking;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
		FGameplayTag OverlayMode = OBM_OverlayModeTags::Default;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
		FGameplayTag LocomotionAction = FGameplayTag::EmptyTag;
};

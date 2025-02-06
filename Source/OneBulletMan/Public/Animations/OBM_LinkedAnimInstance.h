// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "OBM_LinkedAnimInstance.generated.h"

class UOBM_PlayerAnimInstance;
class AOBM_PlayerCharacter;

/**
 * 
 */
UCLASS()
class ONEBULLETMAN_API UOBM_LinkedAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UOBM_LinkedAnimInstance();

	virtual void NativeInitializeAnimation() override;

	virtual void NativeBeginPlay() override;

public:

	// Be very careful when using this function to read your custom variables using the property access system. It is
// safe to use this function to read variables that change only inside UOBM_LinkedAnimInstance::NativeUpdateAnimation()
// because it is guaranteed that this function will be called before parallel animation evaluation. Reading
// variables that change in other functions can be dangerous because they can be changed in the game thread
// at the same time as being read in the worker thread, which can lead to undefined behavior or even a crash.
	UFUNCTION(BlueprintPure, Category = "ALS|Linked Animation Instance",
		Meta = (BlueprintThreadSafe, ReturnDisplayName = "Parent"))
		UOBM_PlayerAnimInstance* GetParent() const;

protected:
	UPROPERTY(VisibleAnywhere, Category = "State", Transient)
		TWeakObjectPtr<UOBM_PlayerAnimInstance> Parent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
		TObjectPtr<AOBM_PlayerCharacter> Character;

};

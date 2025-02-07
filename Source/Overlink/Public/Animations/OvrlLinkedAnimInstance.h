// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "OvrlLinkedAnimInstance.generated.h"

class UOvrlPlayerAnimInstance;
class AOvrlPlayerCharacter;

/**
 * 
 */
UCLASS()
class OVERLINK_API UOvrlLinkedAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UOvrlLinkedAnimInstance();

	virtual void NativeInitializeAnimation() override;

	virtual void NativeBeginPlay() override;

public:

	// Be very careful when using this function to read your custom variables using the property access system. It is
// safe to use this function to read variables that change only inside UOvrlLinkedAnimInstance::NativeUpdateAnimation()
// because it is guaranteed that this function will be called before parallel animation evaluation. Reading
// variables that change in other functions can be dangerous because they can be changed in the game thread
// at the same time as being read in the worker thread, which can lead to undefined behavior or even a crash.
	UFUNCTION(BlueprintPure, Category = "ALS|Linked Animation Instance",
		Meta = (BlueprintThreadSafe, ReturnDisplayName = "Parent"))
		UOvrlPlayerAnimInstance* GetParent() const;

protected:
	UPROPERTY(VisibleAnywhere, Category = "State", Transient)
		TWeakObjectPtr<UOvrlPlayerAnimInstance> Parent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
		TObjectPtr<AOvrlPlayerCharacter> Character;

};

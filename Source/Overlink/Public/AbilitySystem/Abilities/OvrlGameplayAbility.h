// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "OvrlGameplayAbility.generated.h"

class UOvrlAbilityCost;

/**
 * EOvrlAbilityActivationPolicy
 *
 *	Defines how an ability is meant to activate.
 */
UENUM(BlueprintType)
enum class EOvrlAbilityActivationPolicy : uint8
{
	// Try to activate the ability when the input is triggered.
	OnInputTriggered,

	// Continually try to activate the ability while the input is active.
	WhileInputActive,

	// Try to activate the ability when an avatar is assigned.
	OnSpawn
};

UCLASS()
class OVERLINK_API UOvrlGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UOvrlGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:

	EOvrlAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }

	virtual void OnAbilityInputPressed() {};
	virtual void OnAbilityInputReleased() {};

protected:

	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;

	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

protected:

	// Defines how this ability is meant to activate.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ovrl|Ability Activation")
		EOvrlAbilityActivationPolicy ActivationPolicy;

	// Additional costs that must be paid to activate this ability
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Ovrl|Costs")
		TArray<TObjectPtr<UOvrlAbilityCost>> AdditionalCosts;
	
};

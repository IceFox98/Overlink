// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/OBM_GameplayAbility.h"
#include "AbilitySystem/OBM_AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/OBM_AbilityCost.h"


UOBM_GameplayAbility::UOBM_GameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UOBM_GameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	TryActivateAbilityOnSpawn(ActorInfo, Spec);
}

void UOBM_GameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const
{
	if (ActorInfo && !Spec.IsActive() && (ActivationPolicy == EOBM_AbilityActivationPolicy::OnSpawn))
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();

		ASC->TryActivateAbility(Spec.Handle);
	}
}

bool UOBM_GameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags) || !ActorInfo)
	{
		return false;
	}

	// Verify we can afford any additional costs
	for (TObjectPtr<UOBM_AbilityCost> AdditionalCost : AdditionalCosts)
	{
		if (AdditionalCost != nullptr)
		{
			if (!AdditionalCost->CheckCost(this, Handle, ActorInfo, /*inout*/ OptionalRelevantTags))
			{
				return false;
			}
		}
	}

	return true;
}

void UOBM_GameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);

	check(ActorInfo);

	for (TObjectPtr<UOBM_AbilityCost> AdditionalCost : AdditionalCosts)
	{
		AdditionalCost->ApplyCost(this, Handle, ActorInfo, ActivationInfo);
	}
}

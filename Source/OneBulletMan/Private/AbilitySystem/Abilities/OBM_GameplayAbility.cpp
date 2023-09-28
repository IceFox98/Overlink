// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/OBM_GameplayAbility.h"
#include "AbilitySystem/OBM_AbilitySystemComponent.h"

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

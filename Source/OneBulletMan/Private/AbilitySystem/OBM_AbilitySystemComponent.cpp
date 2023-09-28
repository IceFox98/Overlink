// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/OBM_AbilitySystemComponent.h"

void UOBM_AbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UOBM_AbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				AbilitySpecInputReleased(AbilitySpec);
			}
		}
	}
}
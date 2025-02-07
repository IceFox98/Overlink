// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/OvrlAbilitySystemComponent.h"

void UOvrlAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
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

void UOvrlAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
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

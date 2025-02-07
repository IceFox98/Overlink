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
				AbilitySpecInputPressed(AbilitySpec);
				TryActivateAbility(AbilitySpec.Handle);

				// Use replicated events instead so that the WaitInputRelease ability task works.
				// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
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
			AbilitySpec.InputPressed = false;

			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				AbilitySpecInputReleased(AbilitySpec);

				// Use replicated events instead so that the WaitInputRelease ability task works.
				// Invoke the InputReleased event. This is not replicated here. If someone is listening, they may replicate the InputReleased event to the server.
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
			}
		}
	}
}

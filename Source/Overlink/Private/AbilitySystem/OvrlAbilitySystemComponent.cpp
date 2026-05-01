// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/OvrlAbilitySystemComponent.h"

#include "AbilitySystem/Abilities/OvrlGameplayAbility.h"

#include "OvrlLogUtils.h"

void UOvrlAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	for (const FGameplayAbilitySpecHandle& SpecHandle : InputStartedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;
				
				const UOvrlGameplayAbility* OvrlAbilityCDO = Cast<UOvrlGameplayAbility>(AbilitySpec->Ability);
				if (OvrlAbilityCDO && OvrlAbilityCDO->GetActivationPolicy() == EOvrlAbilityActivationPolicy::OnInputStarted)
				{
					AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
				}

				// @TODO: Should I create a AbilitySpecInputStarted()? 
				UGameplayAbility* Instance = AbilitySpec->GetPrimaryInstance();
				if (UOvrlGameplayAbility* OvrlAbilityInstance = Cast<UOvrlGameplayAbility>(Instance))
				{
					OvrlAbilityInstance->OnAbilityInputStarted();
				}
			}
		}
	}

	// for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	// {
	// 	if (const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
	// 	{
	// 		if (AbilitySpec->Ability && !AbilitySpec->IsActive())
	// 		{
	// 			const UOvrlGameplayAbility* OvrlAbilityCDO = Cast<UOvrlGameplayAbility>(AbilitySpec->Ability);
	// 			if (OvrlAbilityCDO && OvrlAbilityCDO->GetActivationPolicy() == EOvrlAbilityActivationPolicy::WhileInputActive)
	// 			{
	// 				AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
	// 			}
	// 		}
	// 	}
	// }

	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;

				if (AbilitySpec->IsActive())
				{
					// Ability is active so pass along the input event.
					AbilitySpecInputPressed(*AbilitySpec);
				}
				else
				{
					const UOvrlGameplayAbility* OvrlAbilityCDO = Cast<UOvrlGameplayAbility>(AbilitySpec->Ability);

					if (OvrlAbilityCDO && OvrlAbilityCDO->GetActivationPolicy() == EOvrlAbilityActivationPolicy::OnInputTriggered)
					{
						AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
					}
				}
			}
		}
	}

	// Try to activate all the abilities that are from presses and holds.
	// We do it all at once so that held inputs don't activate the ability
	// and then also send a input event to the ability because of the press.
	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		TryActivateAbility(AbilitySpecHandle);
	}

	// Process all abilities that had their input released this frame.
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false;

				// Ability is active so pass along the input event.
				AbilitySpecInputReleased(*AbilitySpec);
			}
		}
	}

	InputStartedSpecHandles.Reset();
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void UOvrlAbilitySystemComponent::AddDynamicTagGameplayEffect(const FGameplayTag& Tag)
{
	const TSubclassOf<UGameplayEffect> DynamicTagGE = UOvrlDynamicGameplayEffect::StaticClass();
	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(DynamicTagGE, 1.0f, MakeEffectContext());
	FGameplayEffectSpec* Spec = SpecHandle.Data.Get();

	if (!Spec)
	{
		OVRL_LOG_WARN(LogOverlink, false, "AddDynamicTagGameplayEffect: Unable to make outgoing spec for [%s].", *GetNameSafe(DynamicTagGE));
		return;
	}

	Spec->DynamicGrantedTags.AddTag(Tag);

	ApplyGameplayEffectSpecToSelf(*Spec);
}

void UOvrlAbilitySystemComponent::RemoveDynamicTagGameplayEffect(const FGameplayTag& Tag)
{
	const TSubclassOf<UGameplayEffect> DynamicTagGE = UGameplayEffect::StaticClass();

	FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(FGameplayTagContainer(Tag));
	Query.EffectDefinition = DynamicTagGE;

	RemoveActiveEffects(Query);
}

void UOvrlAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);

	if (Spec.IsActive())
	{
		PRAGMA_DISABLE_DEPRECATION_WARNINGS
		UGameplayAbility* Instance = Spec.GetPrimaryInstance();
		if (UOvrlGameplayAbility* OvrlAbilityInstance = Cast<UOvrlGameplayAbility>(Instance))
		{
			OvrlAbilityInstance->OnAbilityInputPressed();
		}

		FPredictionKey OriginalPredictionKey = Instance ? Instance->GetCurrentActivationInfo().GetActivationPredictionKey() : Spec.ActivationInfo.GetActivationPredictionKey();
		PRAGMA_ENABLE_DEPRECATION_WARNINGS

		// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, OriginalPredictionKey);
	}
}

void UOvrlAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	UGameplayAbility* Instance = Spec.GetPrimaryInstance();
	if (UOvrlGameplayAbility* OvrlAbilityInstance = Cast<UOvrlGameplayAbility>(Instance))
	{
		OvrlAbilityInstance->OnAbilityInputReleased();
	}

	FPredictionKey OriginalPredictionKey = Instance ? Instance->GetCurrentActivationInfo().GetActivationPredictionKey() : Spec.ActivationInfo.GetActivationPredictionKey();

	// Invoke the InputReleased event. This is not replicated here. If someone is listening, they may replicate the InputReleased event to the server.
	InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, OriginalPredictionKey);
	PRAGMA_ENABLE_DEPRECATION_WARNINGS
}

void UOvrlAbilitySystemComponent::NotifyAbilityFailed(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
	Super::NotifyAbilityFailed(Handle, Ability, FailureReason);

	if (UOvrlGameplayAbility* OvrlAbility = Cast<UOvrlGameplayAbility>(Ability))
	{
		OvrlAbility->OnAbilityFailedToActivate(FailureReason);
	}
}

void UOvrlAbilitySystemComponent::AbilityInputTagStarted(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				InputStartedSpecHandles.Add(AbilitySpec.Handle);
			}
		}
	}
}

void UOvrlAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
			}
		}
	}
}

void UOvrlAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
			{
				InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputStartedSpecHandles.Remove(AbilitySpec.Handle);
				InputHeldSpecHandles.Remove(AbilitySpec.Handle);
			}
		}
	}
}

// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/OvrlGameplayAbility.h"
#include "AbilitySystem/OvrlAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/OvrlAbilityCost.h"
#include "GameFramework/Character.h"

UOvrlGameplayAbility::UOvrlGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	bAllowOnMontagePlay = true;
}

void UOvrlGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!bAllowOnMontagePlay)
	{
		if (UAnimInstance* AnimInstance = GetOwnerAnimInstance())
		{
			AnimInstance->OnMontageStarted.AddUniqueDynamic(this, &UOvrlGameplayAbility::OnAnimMontageStarted);
		}
	}
}

void UOvrlGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	if (!bAllowOnMontagePlay)
	{
		if (UAnimInstance* AnimInstance = GetOwnerAnimInstance())
		{
			AnimInstance->OnMontageStarted.RemoveDynamic(this, &UOvrlGameplayAbility::OnAnimMontageStarted);
		}
	}
}

bool UOvrlGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	const bool bCanActivate = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
	
	if (!bAllowOnMontagePlay)
	{
		if (UAnimInstance* AnimInstance = GetOwnerAnimInstance())
		{
			return bCanActivate && !AnimInstance->IsAnyMontagePlaying();
		}
	}
	
	return bCanActivate;
}

void UOvrlGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	TryActivateAbilityOnSpawn(ActorInfo, Spec);
}

void UOvrlGameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const
{
	if (ActorInfo && !Spec.IsActive() && (ActivationPolicy == EOvrlAbilityActivationPolicy::OnSpawn))
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();

		ASC->TryActivateAbility(Spec.Handle);
	}
}

void UOvrlGameplayAbility::OnAbilityInputPressed()
{
}

void UOvrlGameplayAbility::OnAbilityInputReleased()
{
}

void UOvrlGameplayAbility::OnAbilityFailedToActivate_Implementation(const FGameplayTagContainer& FailedReason) const
{
}

bool UOvrlGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags) || !ActorInfo)
	{
		return false;
	}

	// Verify we can afford any additional costs
	for (TObjectPtr<UOvrlAbilityCost> AdditionalCost : AdditionalCosts)
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

void UOvrlGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);

	check(ActorInfo);

	for (TObjectPtr<UOvrlAbilityCost> AdditionalCost : AdditionalCosts)
	{
		AdditionalCost->ApplyCost(this, Handle, ActorInfo, ActivationInfo);
	}
}

void UOvrlGameplayAbility::OnAnimMontageStarted(UAnimMontage* Montage)
{
	CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
}

UAnimInstance* UOvrlGameplayAbility::GetOwnerAnimInstance() const
{
	if (ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwningActorFromActorInfo()))
	{
		if (USkeletalMeshComponent* SkeletalMesh = OwnerCharacter->GetMesh())
		{
			return SkeletalMesh->GetAnimInstance();
		}
	}

	return nullptr;
}

// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Attributes/OvrlHealthSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

#include "OvrlGameplayTags.h"

void UOvrlHealthSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UOvrlHealthSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UOvrlHealthSet, MaxHealth, COND_None, REPNOTIFY_Always);
}

UOvrlHealthSet::UOvrlHealthSet()
	: Health(100.0f)
	  , MaxHealth(100.0f)
{
}

void UOvrlHealthSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOvrlHealthSet, Health, OldValue);
}

void UOvrlHealthSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UOvrlHealthSet, MaxHealth, OldValue);
}

void UOvrlHealthSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UOvrlHealthSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
		if (ASC && ASC->HasMatchingGameplayTag(OvrlCheatTags::UnlimitedHealth))
		{
			// In this way, the gameplay effect will still be applied, but the health will not change.
			NewValue = GetHealth();
			return;
		}
	}

	ClampAttribute(Attribute, NewValue);
}

void UOvrlHealthSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		// Do not allow health to go negative or above max health.
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		// Do not allow max health to drop below 1.
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}

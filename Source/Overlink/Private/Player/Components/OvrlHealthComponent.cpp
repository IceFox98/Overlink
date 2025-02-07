// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Components/OvrlHealthComponent.h"

#include "AbilitySystem/Attributes/OvrlHealthSet.h"
#include "AbilitySystem/OvrlAbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffectExtension.h"

#include "OvrlUtils.h"

// Sets default values for this component's properties
UOvrlHealthComponent::UOvrlHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	bIsDead = false;
}

static AActor* GetInstigatorFromAttrChangeData(const FOnAttributeChangeData& ChangeData)
{
	if (ChangeData.GEModData != nullptr)
	{
		const FGameplayEffectContextHandle& EffectContext = ChangeData.GEModData->EffectSpec.GetEffectContext();
		return EffectContext.GetOriginalInstigator();
	}

	return nullptr;
}

void UOvrlHealthComponent::InitializeWithASC(UOvrlAbilitySystemComponent* ASC)
{
	AActor* Owner = GetOwner();
	check(Owner);

	if (AbilitySystemComponent)
	{
		OVRL_LOG_ERR(LogTemp, true, "Health component for owner [%s] has already been initialized with an ability system.", *GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent = ASC;
	if (!AbilitySystemComponent)
	{
		OVRL_LOG_ERR(LogTemp, true, "Cannot initialize health component for owner [%s] with NULL ability system.", *GetNameSafe(Owner));
		return;
	}

	HealthSet = AbilitySystemComponent->AddSet<UOvrlHealthSet>();
	if (!HealthSet)
	{
		OVRL_LOG_ERR(LogTemp, true, "Cannot initialize health component for owner [%s] with NULL health set on the ability system.", *GetNameSafe(Owner));
		return;
	}

	// Register ASC delegates
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UOvrlHealthSet::GetHealthAttribute()).AddUObject(this, &UOvrlHealthComponent::HandleHealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UOvrlHealthSet::GetMaxHealthAttribute()).AddUObject(this, &UOvrlHealthComponent::HandleMaxHealthChanged);
}

void UOvrlHealthComponent::HandleHealthChanged(const FOnAttributeChangeData& Data)
{
	OnHealthChanged.Broadcast(this, Data.OldValue, Data.NewValue);

	if (Data.NewValue <= 0.f)
	{
		OnOutOfHealth.Broadcast(GetInstigatorFromAttrChangeData(Data));
	}
}

void UOvrlHealthComponent::HandleMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	OnMaxHealthChanged.Broadcast(this, Data.OldValue, Data.NewValue);
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Components/HealthComponent.h"

#include "AbilitySystem/Attributes/OBM_HealthSet.h"
#include "AbilitySystem/OBM_AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffectExtension.h"

#include "OBM_Utils.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
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

void UHealthComponent::InitializeWithASC(UOBM_AbilitySystemComponent* ASC)
{
	AActor* Owner = GetOwner();
	check(Owner);

	if (AbilitySystemComponent)
	{
		OBM_LOG_ERR(LogTemp, true, "Health component for owner [%s] has already been initialized with an ability system.", *GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent = ASC;
	if (!AbilitySystemComponent)
	{
		OBM_LOG_ERR(LogTemp, true, "Cannot initialize health component for owner [%s] with NULL ability system.", *GetNameSafe(Owner));
		return;
	}

	HealthSet = AbilitySystemComponent->AddSet<UOBM_HealthSet>();
	if (!HealthSet)
	{
		OBM_LOG_ERR(LogTemp, true, "Cannot initialize health component for owner [%s] with NULL health set on the ability system.", *GetNameSafe(Owner));
		return;
	}

	// Register ASC delegates
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UOBM_HealthSet::GetHealthAttribute()).AddUObject(this, &UHealthComponent::HandleHealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UOBM_HealthSet::GetMaxHealthAttribute()).AddUObject(this, &UHealthComponent::HandleMaxHealthChanged);
}

void UHealthComponent::HandleHealthChanged(const FOnAttributeChangeData& Data)
{
	OnHealthChanged.Broadcast(this, Data.OldValue, Data.NewValue);

	if (Data.NewValue <= 0.f)
	{
		OnOutOfHealth.Broadcast(GetInstigatorFromAttrChangeData(Data));
	}
}

void UHealthComponent::HandleMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	OnMaxHealthChanged.Broadcast(this, Data.OldValue, Data.NewValue);
}


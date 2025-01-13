// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Components/OBM_HealthComponent.h"

#include "AbilitySystem/Attributes/OBM_HealthSet.h"
#include "AbilitySystem/OBM_AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffectExtension.h"

#include "OBM_Utils.h"

// Sets default values for this component's properties
UOBM_HealthComponent::UOBM_HealthComponent()
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

void UOBM_HealthComponent::InitializeWithASC(UOBM_AbilitySystemComponent* ASC)
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
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UOBM_HealthSet::GetHealthAttribute()).AddUObject(this, &UOBM_HealthComponent::HandleHealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UOBM_HealthSet::GetMaxHealthAttribute()).AddUObject(this, &UOBM_HealthComponent::HandleMaxHealthChanged);
}

void UOBM_HealthComponent::HandleHealthChanged(const FOnAttributeChangeData& Data)
{
	OnHealthChanged.Broadcast(this, Data.OldValue, Data.NewValue);

	if (Data.NewValue <= 0.f)
	{
		OnOutOfHealth.Broadcast(GetInstigatorFromAttrChangeData(Data));
	}
}

void UOBM_HealthComponent::HandleMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	OnMaxHealthChanged.Broadcast(this, Data.OldValue, Data.NewValue);
}


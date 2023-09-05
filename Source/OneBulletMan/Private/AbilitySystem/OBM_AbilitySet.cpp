// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/OBM_AbilitySet.h"
#include "AbilitySystem/OBM_AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/OBM_GameplayAbility.h"

void FOBM_AbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.AddUnique(Handle);
	}
}

void FOBM_AbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.AddUnique(Handle);
	}
}

void FOBM_AbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* Set)
{
	GrantedAttributeSets.AddUnique(Set);
}

void FOBM_AbilitySet_GrantedHandles::TakeFromAbilitySystem(UOBM_AbilitySystemComponent* OBM_ASC)
{
	check(OBM_ASC);

	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			OBM_ASC->ClearAbility(Handle);
		}
	}

	for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			OBM_ASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	for (UAttributeSet* Set : GrantedAttributeSets)
	{
		OBM_ASC->GetSpawnedAttributes_Mutable().Remove(Set);
	}

	AbilitySpecHandles.Reset();
	GameplayEffectHandles.Reset();
	GrantedAttributeSets.Reset();
}

UOBM_AbilitySet::UOBM_AbilitySet(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UOBM_AbilitySet::GiveToAbilitySystem(UOBM_AbilitySystemComponent* OBM_ASC, FOBM_AbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
	check(OBM_ASC);

	// Grant the gameplay abilities.
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FOBM_AbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability))
		{
			UE_LOG(LogTemp, Error, TEXT("GrantedGameplayAbilities[%d] on ability set [%s] is not valid."), AbilityIndex, *GetNameSafe(this));
			continue;
		}

		UOBM_GameplayAbility* OBM_Ability = AbilityToGrant.Ability->GetDefaultObject<UOBM_GameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(OBM_Ability, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

		// Give the Ability to ASC and save the returned handleID
		const FGameplayAbilitySpecHandle AbilitySpecHandle = OBM_ASC->GiveAbility(AbilitySpec);

		if (OutGrantedHandles)
		{
			// Add the handle to the handles list of the Item
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}

	// Grant the gameplay effects.
	for (int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); ++EffectIndex)
	{
		const FOBM_AbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

		if (!IsValid(EffectToGrant.GameplayEffect))
		{
			UE_LOG(LogTemp, Error, TEXT("GrantedGameplayEffects[%d] on ability set [%s] is not valid"), EffectIndex, *GetNameSafe(this));
			continue;
		}

		const UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle GameplayEffectHandle = OBM_ASC->ApplyGameplayEffectToSelf(GameplayEffect, EffectToGrant.EffectLevel, OBM_ASC->MakeEffectContext());

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
		}
	}

	// Grant the attribute sets.
	for (int32 SetIndex = 0; SetIndex < GrantedAttributes.Num(); ++SetIndex)
	{
		const FOBM_AbilitySet_AttributeSet& SetToGrant = GrantedAttributes[SetIndex];

		if (!IsValid(SetToGrant.AttributeSet))
		{
			UE_LOG(LogTemp, Error, TEXT("GrantedAttributes[%d] on ability set [%s] is not valid"), SetIndex, *GetNameSafe(this));
			continue;
		}

		UAttributeSet* NewSet = NewObject<UAttributeSet>(OBM_ASC->GetOwner(), SetToGrant.AttributeSet);
		OBM_ASC->AddAttributeSetSubobject(NewSet);

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAttributeSet(NewSet);
		}
	}
}

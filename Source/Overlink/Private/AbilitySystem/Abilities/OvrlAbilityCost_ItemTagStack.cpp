// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/OBM_AbilityCost_ItemTagStack.h"

#include "AbilitySystem/Abilities/OBM_GameplayAbility.h"
#include "NativeGameplayTags.h"
#include "Equipment/OBM_EquipmentInstance.h"
#include "Inventory/OBM_ItemInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OBM_AbilityCost_ItemTagStack)

UE_DEFINE_GAMEPLAY_TAG(TAG_ABILITY_FAIL_COST, "Ability.ActivateFail.Cost");

UOBM_AbilityCost_ItemTagStack::UOBM_AbilityCost_ItemTagStack()
{
	Quantity = 1.f;
	FailureTag = TAG_ABILITY_FAIL_COST;
}

bool UOBM_AbilityCost_ItemTagStack::CheckCost(const UOBM_GameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (FGameplayAbilitySpec* Spec = Ability->GetCurrentAbilitySpec())
	{
		if (AOBM_EquipmentInstance* EquipmentInstance = Cast<AOBM_EquipmentInstance>(Spec->SourceObject.Get()))
		{
			if (UOBM_ItemInstance* ItemInstance = EquipmentInstance->GetAssociatedItem())
			{
				const bool bCanApplyCost = ItemInstance->GetTagStackCount(Tag) >= Quantity;

				// Inform other abilities why this cost cannot be applied
				if (!bCanApplyCost && OptionalRelevantTags && FailureTag.IsValid())
				{
					OptionalRelevantTags->AddTag(FailureTag);
				}

				return bCanApplyCost;
			}
		}
	}

	return false;
}

void UOBM_AbilityCost_ItemTagStack::ApplyCost(const UOBM_GameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (FGameplayAbilitySpec* Spec = Ability->GetCurrentAbilitySpec())
	{
		if (AOBM_EquipmentInstance* EquipmentInstance = Cast<AOBM_EquipmentInstance>(Spec->SourceObject.Get()))
		{
			if (UOBM_ItemInstance* ItemInstance = EquipmentInstance->GetAssociatedItem())
			{
				ItemInstance->RemoveStack(Tag, Quantity);
			}
		}
	}
}

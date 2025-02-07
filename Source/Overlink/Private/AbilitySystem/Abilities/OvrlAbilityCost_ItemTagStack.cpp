// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/OvrlAbilityCost_ItemTagStack.h"

#include "AbilitySystem/Abilities/OvrlGameplayAbility.h"
#include "NativeGameplayTags.h"
#include "Equipment/OvrlEquipmentInstance.h"
#include "Inventory/OvrlItemInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OvrlAbilityCost_ItemTagStack)

UE_DEFINE_GAMEPLAY_TAG(TAG_ABILITY_FAIL_COST, "Ability.ActivateFail.Cost");

UOvrlAbilityCost_ItemTagStack::UOvrlAbilityCost_ItemTagStack()
{
	Quantity = 1.f;
	FailureTag = TAG_ABILITY_FAIL_COST;
}

bool UOvrlAbilityCost_ItemTagStack::CheckCost(const UOvrlGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (FGameplayAbilitySpec* Spec = Ability->GetCurrentAbilitySpec())
	{
		if (AOvrlEquipmentInstance* EquipmentInstance = Cast<AOvrlEquipmentInstance>(Spec->SourceObject.Get()))
		{
			if (UOvrlItemInstance* ItemInstance = EquipmentInstance->GetAssociatedItem())
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

void UOvrlAbilityCost_ItemTagStack::ApplyCost(const UOvrlGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (FGameplayAbilitySpec* Spec = Ability->GetCurrentAbilitySpec())
	{
		if (AOvrlEquipmentInstance* EquipmentInstance = Cast<AOvrlEquipmentInstance>(Spec->SourceObject.Get()))
		{
			if (UOvrlItemInstance* ItemInstance = EquipmentInstance->GetAssociatedItem())
			{
				ItemInstance->RemoveStack(Tag, Quantity);
			}
		}
	}
}

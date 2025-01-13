
#include "Inventory/OBM_InventoryComponent.h"

#include "Inventory/OBM_ItemInstance.h"
#include "Inventory/OBM_ItemDefinition.h"
#include "Inventory/OBM_ItemFragment_EquippableItem.h"
#include "Equipment/OBM_EquipmentInstance.h"
#include "Equipment/OBM_EquipmentDefinition.h"

#include "AbilitySystem/OBM_AbilitySystemComponent.h"
#include "AbilitySystem/OBM_AbilitySet.h"

#include "AbilitySystemGlobals.h"

UOBM_InventoryComponent::UOBM_InventoryComponent()
{
}

UOBM_ItemInstance* UOBM_InventoryComponent::AddItemDefinition(TSubclassOf<UOBM_ItemDefinition> ItemDef, int32 StackCount)
{
	UOBM_ItemInstance* ItemInstance = nullptr;

	ItemInstance = NewObject<UOBM_ItemInstance>(GetOwner());
	ItemInstance->SetItemDef(ItemDef);

	// Instantiate the item fragments
	for (UOBM_ItemFragment* Fragment : GetDefault<UOBM_ItemDefinition>(ItemDef)->Fragments)
	{
		if (Fragment)
		{
			Fragment->OnInstanceCreated(ItemInstance);
		}
	}

	// Spawn Item if equippable
	if (const UOBM_ItemFragment_EquippableItem* EquipInfo = ItemInstance->FindFragmentByClass<UOBM_ItemFragment_EquippableItem>())
	{
		TSubclassOf<UOBM_EquipmentDefinition> EquipDefClass = EquipInfo->EquipmentDefinition;
		if (EquipDefClass)
		{
			const UOBM_EquipmentDefinition* EquipmentDef = GetDefault<UOBM_EquipmentDefinition>(EquipDefClass);

			AOBM_EquipmentInstance* EquippedItem = GetWorld()->SpawnActor<AOBM_EquipmentInstance>(EquipmentDef->InstanceType);
			EquippedItem->EquipmentDefinition = EquipDefClass;
			EquippedItem->AssociatedItem = ItemInstance;
			EquippedItem->SetOwner(GetOwner());
			EquippedItem->SetInstigator(Cast<APawn>(GetOwner()));

			EquippedItems.Emplace(EquippedItem);
		}
	}

	SetActiveSlotIndex(EquippedItems.Num() - 1);

	return ItemInstance;
}

UOBM_AbilitySystemComponent* UOBM_InventoryComponent::GetAbilitySystemComponent() const
{
	return Cast<UOBM_AbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()));
}

void UOBM_InventoryComponent::SetActiveSlotIndex(int32 NewIndex)
{
	UnequipItemInSlot();

	SelectedIndex = NewIndex;

	EquipItemInSlot();
}

void UOBM_InventoryComponent::EquipItemInSlot()
{
	// You can equip a new Item only if there's no current equipped item.
	// Be sure to call UnequipCurrentItem first
	if (!SelectedItem && EquippedItems.IsValidIndex(SelectedIndex))
	{
		AOBM_EquipmentInstance* EquipInstance = EquippedItems[SelectedIndex];
		EquipInstance->OnEquipped();

		const UOBM_EquipmentDefinition* EquipmentDef = GetDefault<UOBM_EquipmentDefinition>(EquipInstance->EquipmentDefinition);

		if (UOBM_AbilitySystemComponent* ASC = GetAbilitySystemComponent())
		{
			// When the item is equipped, we give all its abilities/effects/attributes to player's ASC
			for (TObjectPtr<const UOBM_AbilitySet> AbilitySet : EquipmentDef->AbilitySetsToGrant)
			{
				AbilitySet->GiveToAbilitySystem(ASC, /*inout*/ &EquipInstance->GrantedHandles, EquipInstance);
			}
		}

		SelectedItem = EquipInstance;
	}
}

void UOBM_InventoryComponent::UnequipItemInSlot()
{
	if (SelectedItem)
	{
		SelectedItem->OnUnequipped();

		if (UOBM_AbilitySystemComponent* ASC = GetAbilitySystemComponent())
		{
			// When unequip the item, remove all given abilities/effects/attributes from player's ASC
			SelectedItem->GrantedHandles.TakeFromAbilitySystem(ASC);
		}

		SelectedItem = nullptr;
	}
}

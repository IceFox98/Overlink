
#include "Inventory/InventoryComponent.h"
#include "Inventory/ItemInstance.h"
#include "Inventory/ItemDefinition.h"
#include "Inventory/ItemFragment_EquippableItem.h"
#include "Equipment/EquipmentInstance.h"
#include "Equipment/EquipmentDefinition.h"

#include "AbilitySystem/OBM_AbilitySystemComponent.h"
#include "AbilitySystem/OBM_AbilitySet.h"

#include "AbilitySystemGlobals.h"

UInventoryComponent::UInventoryComponent()
{
}

UItemInstance* UInventoryComponent::AddItemDefinition(TSubclassOf<UItemDefinition> ItemDef, int32 StackCount)
{
	UItemInstance* ItemInstance = nullptr;

	ItemInstance = NewObject<UItemInstance>(GetOwner());
	ItemInstance->SetItemDef(ItemDef);

	// Instantiate the item fragments
	for (UItemFragment* Fragment : GetDefault<UItemDefinition>(ItemDef)->Fragments)
	{
		if (Fragment)
		{
			Fragment->OnInstanceCreated(ItemInstance);
		}
	}

	// Spawn Item if equippable
	if (const UItemFragment_EquippableItem* EquipInfo = ItemInstance->FindFragmentByClass<UItemFragment_EquippableItem>())
	{
		TSubclassOf<UEquipmentDefinition> EquipDefClass = EquipInfo->EquipmentDefinition;
		if (EquipDefClass)
		{
			const UEquipmentDefinition* EquipmentDef = GetDefault<UEquipmentDefinition>(EquipDefClass);

			AEquipmentInstance* EquippedItem = GetWorld()->SpawnActor<AEquipmentInstance>(EquipmentDef->InstanceType);
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

UOBM_AbilitySystemComponent* UInventoryComponent::GetAbilitySystemComponent() const
{
	return Cast<UOBM_AbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()));
}

void UInventoryComponent::SetActiveSlotIndex(int32 NewIndex)
{
	UnequipItemInSlot();

	SelectedIndex = NewIndex;

	EquipItemInSlot();
}

void UInventoryComponent::EquipItemInSlot()
{
	// You can equip a new Item only if there's no current equipped item.
	// Be sure to call UnequipCurrentItem first
	if (!SelectedItem && EquippedItems.IsValidIndex(SelectedIndex))
	{
		AEquipmentInstance* EquipInstance = EquippedItems[SelectedIndex];
		EquipInstance->OnEquipped();

		const UEquipmentDefinition* EquipmentDef = GetDefault<UEquipmentDefinition>(EquipInstance->EquipmentDefinition);

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

void UInventoryComponent::UnequipItemInSlot()
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

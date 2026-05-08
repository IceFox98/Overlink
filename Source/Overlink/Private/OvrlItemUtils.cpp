// Fill out your copyright notice in the Description page of Project Settings.

#include "OvrlItemUtils.h"

// Internal
#include "Inventory/OvrlInventoryComponent.h"
#include "Inventory/OvrlItemDefinition.h"
#include "Inventory/OvrlItemFragment_PickupableItem.h"
#include "Inventory/OvrlItemFragment_SetStats.h"
#include "Inventory/OvrlItemFragment_EquippableItem.h"
#include "Equipment/OvrlEquipmentDefinition.h"
#include "Inventory/OvrlItemInstance.h"
#include "Player/Components/OvrlEquipmentManagerComponent.h"
#include "OvrlLogUtils.h"


FOvrlItemEntry UOvrlItemUtils::GetFirstItemEntry(AActor* InventoryOwner, TSubclassOf<UOvrlItemDefinition> ItemDefinition)
{
	if (InventoryOwner)
	{
		if (UOvrlInventoryComponent* Inventory = Cast<UOvrlInventoryComponent>(InventoryOwner->GetComponentByClass(UOvrlInventoryComponent::StaticClass())))
		{
			return Inventory->FindFirstItemEntryByDefinition(ItemDefinition);
		}
	}

	return FOvrlItemEntry();
}

AOvrlEquipmentInstance* UOvrlItemUtils::GetFirstEquipmentInstance(AActor* ManagerOwner, TSubclassOf<UOvrlItemDefinition> ItemDefinition)
{
	if (ManagerOwner)
	{
		if (UOvrlEquipmentManagerComponent* EquipmentManagerComponent =
			Cast<UOvrlEquipmentManagerComponent>(ManagerOwner->GetComponentByClass(UOvrlEquipmentManagerComponent::StaticClass())))
		{
			return EquipmentManagerComponent->FindFirstQuickSlotEntryByDefinition(ItemDefinition).EquipmentInstance;
		}
	}

	return nullptr;
}

const UOvrlEquipmentDefinition* UOvrlItemUtils::GetItemEquipmentDefinition(const UOvrlItemInstance* Item)
{
	if (!Item)
	{
		return nullptr;
	}

	if (const UOvrlItemFragment_EquippableItem* EquipFragment = Item->FindFragmentByClass<UOvrlItemFragment_EquippableItem>())
	{
		TSubclassOf<UOvrlEquipmentDefinition> EquipDefClass = EquipFragment->EquipmentDefinition;
		if (EquipDefClass)
		{
			return GetDefault<UOvrlEquipmentDefinition>(EquipDefClass);
		}
	}

	return nullptr;
}

int32 UOvrlItemUtils::GetDefaultStatFromItemDef(const TSubclassOf<UOvrlItemDefinition> WeaponItemClass, FGameplayTag StatTag)
{
	if (WeaponItemClass)
	{
		if (UOvrlItemDefinition* WeaponItemCDO = WeaponItemClass->GetDefaultObject<UOvrlItemDefinition>())
		{
			if (const UOvrlItemFragment_SetStats* ItemStatsFragment = WeaponItemCDO->FindFragmentByClass<UOvrlItemFragment_SetStats>())
			{
				return ItemStatsFragment->GetItemStatByTag(StatTag);
			}
		}
	}

	return 0;
}

UOvrlPickupDefinition* UOvrlItemUtils::GetPickupDefinitionFromItemDefinition(TSubclassOf<UOvrlItemDefinition> ItemDefinitionClass)
{
	if (!ItemDefinitionClass)
	{
		OVRL_LOG_ERR(LogOverlink, true, "Failed to find Pickup Definition, ItemDefinitionClass is not valid");
		return nullptr;
	}
	
	UOvrlItemDefinition* ItemDefinition = Cast<UOvrlItemDefinition>(ItemDefinitionClass->GetDefaultObject());

	if (const UOvrlItemFragment_PickupableItem* PickupableItemFragment = ItemDefinition->FindFragmentByClass<UOvrlItemFragment_PickupableItem>())
	{
		return PickupableItemFragment->PickupDefinition;
	}
	
	OVRL_LOG_WARN(LogOverlink, true, "Failed to find Pickup Definition. ItemDefinition '%s' must have a PickupableItem fragment.", *ItemDefinition->GetName())
	return nullptr;
}

UOvrlPickupDefinition* UOvrlItemUtils::GetPickupDefinitionFromItemInstance(const UOvrlItemInstance* ItemInstance)
{
	if (!ItemInstance)
	{
		OVRL_LOG_ERR(LogOverlink, true, "Failed to find Pickup Definition, ItemInstance is not valid");
		return nullptr;
	}
	
	if (const UOvrlItemFragment_PickupableItem* PickupableItemFragment = ItemInstance->FindFragmentByClass<UOvrlItemFragment_PickupableItem>())
	{
		return PickupableItemFragment->PickupDefinition;
	}
	
	OVRL_LOG_WARN(LogOverlink, true, "Failed to find Pickup Definition. ItemInstance '%s' (ID: '%s') must have a PickupableItem fragment.", *ItemInstance->GetName(), *ItemInstance->GetItemDef()->GetName())
	return nullptr;
}

// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/Components/OvrlEquipmentManagerComponent.h"

#include "AbilitySystemGlobals.h"
#include "Equipment/OvrlEquipmentDefinition.h"
#include "Equipment/OvrlEquipmentInstance.h"
#include "Inventory/OvrlInventoryComponent.h"
#include "Inventory/OvrlItemFragment_EquippableItem.h"
#include "Inventory/OvrlItemInstance.h"
#include "AbilitySystem/OvrlAbilitySystemComponent.h"
#include "OvrlLogUtils.h"
#include "OvrlItemUtils.h"

void UOvrlEquipmentManagerComponent::InitializeFromInventory(UOvrlInventoryComponent* InInventoryComponent)
{
	InventoryComponent = InInventoryComponent;

	if (InventoryComponent)
	{
		InventoryComponent->OnItemRemoved.AddUniqueDynamic(this, &UOvrlEquipmentManagerComponent::OnInventoryItemRemoved);
		InventoryComponent->OnItemAdded.AddUniqueDynamic(this, &UOvrlEquipmentManagerComponent::OnInventoryItemAdded);
	}

	QuickSlotEntries.Init(FQuickSlotEntry(), NumQuickSlots);
}

bool UOvrlEquipmentManagerComponent::HasAnyQuickSlotAvailable() const
{
	return GetFirstAvailableQuickSlotIndex() != INDEX_NONE;
}

void UOvrlEquipmentManagerComponent::OnInventoryItemAdded(UOvrlItemInstance* AddedItem)
{
	// Check if it's equippable
	if (const UOvrlEquipmentDefinition* EquipmentDef = UOvrlItemUtils::GetItemEquipmentDefinition(AddedItem))
	{
		// Should be equipped immediately?
		if (EquipmentDef->bSetAsActiveSlotOnAdded && EquipmentDef->bShouldSpawnEquipmentInstance)
		{
			bool bForceSet = false;
			if (!HasAnyQuickSlotAvailable())
			{
				if (CurrentActiveSlotEntry.ItemInstance)
				{
					// Replace the current active item with the new one.
					// This will trigger OnItemRemoved delegate
					InventoryComponent->DropItem(CurrentActiveSlotEntry.ItemInstance);
				}

				// If we're here it means that there's no quick slot left, so we force re-select the new added item
				// to the currently active slot.
				bForceSet = true;
			}

			const int32 SlotIndex = AddItemToQuickSlots(AddedItem);
			
			if (!bForceSet)
			{
				// Force set active index if the new item's index is the same as the current one.
				bForceSet = SlotIndex == QuickSlotIndex;
			}
			
			SetActiveSlotIndex(SlotIndex, bForceSet);
		}
	}
}

void UOvrlEquipmentManagerComponent::SetActiveSlotIndex(int32 NewIndex, bool bForceSet/* = false*/)
{
	if (!bForceSet)
	{
		if (QuickSlotIndex == NewIndex || TimerHandle_EquipItem.IsValid())
		{
			// Do nothing if index is the same.
			return;
		}
	}

	AOvrlEquipmentInstance* NewEquipInstance = GetOrSpawnEquipmentInstance(NewIndex);
	if (NewEquipInstance)
	{
		if (CurrentActiveSlotEntry.EquipmentInstance)
		{
			CurrentActiveSlotEntry.EquipmentInstance->OnBeforeUnequip(); // Prepare current item to unequip (stop firing/animations...)
		}

		const float EquipNotifyTime = NewEquipInstance->GetEquipNotifyTime();
		NewEquipInstance->PlayEquipMontage();

		if (EquipNotifyTime > 0.f)
		{
			// Simulate weapon switch animation, but actually perform the switch only after specific amount of time
			FTimerDelegate TimerDelegate;
			TimerDelegate.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(UOvrlEquipmentManagerComponent, SetActiveSlotIndex_Internal), NewIndex);
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_EquipItem, TimerDelegate, EquipNotifyTime, false);
		}
		else
		{
			// Instant item switch
			SetActiveSlotIndex_Internal(NewIndex);
		}
	}
}

void UOvrlEquipmentManagerComponent::SetActiveSlotIndex_Internal(int32 NewIndex)
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_EquipItem);

	UnequipItemInSlot();

	QuickSlotIndex = NewIndex;

	EquipItemInSlot();
}

bool UOvrlEquipmentManagerComponent::SelectNearestValidSlot()
{
	for (int32 Offset = 1; Offset < NumQuickSlots; Offset++)
	{
		const int32 RightIndex = QuickSlotIndex + Offset;
		if (QuickSlotEntries.IsValidIndex(RightIndex) && QuickSlotEntries[RightIndex].ItemInstance)
		{
			SetActiveSlotIndex(RightIndex);
			return true;
		}

		const int32 LeftIndex = QuickSlotIndex - Offset;
		if (QuickSlotEntries.IsValidIndex(LeftIndex) && QuickSlotEntries[LeftIndex].ItemInstance)
		{
			SetActiveSlotIndex(LeftIndex);
			return true;
		}
	}
	
	// Nothing changed
	return false;
}

AOvrlEquipmentInstance* UOvrlEquipmentManagerComponent::GetOrSpawnEquipmentInstance(int32 Index)
{
	if (!QuickSlotEntries.IsValidIndex(Index))
	{
		return nullptr;
	}

	AOvrlEquipmentInstance* EquipmentInstance = QuickSlotEntries[Index].EquipmentInstance;
	if (!EquipmentInstance)
	{
		UOvrlItemInstance* Item = QuickSlotEntries[Index].ItemInstance;

		if (const UOvrlEquipmentDefinition* EquipmentDefinition = UOvrlItemUtils::GetItemEquipmentDefinition(Item))
		{
			if (EquipmentDefinition->bShouldSpawnEquipmentInstance)
			{
				if (EquipmentDefinition->EquipmentClass)
				{
					FActorSpawnParameters SpawnParams;
					SpawnParams.Owner = GetOwner();
					SpawnParams.Instigator = Cast<APawn>(GetOwner());

					EquipmentInstance = GetWorld()->SpawnActor<AOvrlEquipmentInstance>(EquipmentDefinition->EquipmentClass, SpawnParams);
					EquipmentInstance->Initialize(const_cast<UOvrlEquipmentDefinition*>(EquipmentDefinition), Item);

					QuickSlotEntries[Index].EquipmentInstance = EquipmentInstance;
				}
				else
				{
					OVRL_LOG_WARN(LogOverlink, true, "Tried to spawn equipment instance, but EquipmentClass was not valid! Asset: %s", *EquipmentDefinition->GetName());
				}
			}
		}
	}

	return EquipmentInstance;
}

void UOvrlEquipmentManagerComponent::EquipItem(UOvrlItemInstance* Item)
{
	if (!Item)
	{
		return;
	}

	EquippedItems.Add(Item);

	if (UOvrlAbilitySystemComponent* ASC = GetOwnerAbilitySystemComponent())
	{
		if (const UOvrlEquipmentDefinition* EquipmentDefinition = UOvrlItemUtils::GetItemEquipmentDefinition(Item))
		{
			if (EquipmentDefinition->bShouldSpawnEquipmentInstance && CurrentActiveSlotEntry.ItemInstance == Item)
			{
				if (CurrentActiveSlotEntry.EquipmentInstance)
				{
					for (TObjectPtr<const UOvrlAbilitySet> AbilitySet : EquipmentDefinition->AbilitySetsToGrantToItem)
					{
						// When the item is equipped, we give all its abilities/effects/attributes to the item itself.
						// Useful to give abilities like weapon fire/reload.
						AbilitySet->GiveToAbilitySystem(ASC, /*inout*/ &Item->GetGrantedHandles(), CurrentActiveSlotEntry.EquipmentInstance);
					}
				}
			}

			// When the item is equipped, we give all its Abilities/Effects/Attributes to its owner's ASC
			for (TObjectPtr<const UOvrlAbilitySet> AbilitySet : EquipmentDefinition->AbilitySetsToGrantToOwner)
			{
				AbilitySet->GiveToAbilitySystem(ASC, /*inout*/ &Item->GetGrantedHandles(), Item->GetOuter());
			}
		}
	}

	OnItemEquipped.Broadcast(Item);
}

void UOvrlEquipmentManagerComponent::EquipItemInSlot()
{
	// You can equip a new Item only if there's no current equipped item.
	// Be sure to call UnequipCurrentItem first
	if (!CurrentActiveSlotEntry.EquipmentInstance && QuickSlotEntries.IsValidIndex(QuickSlotIndex))
	{
		AOvrlEquipmentInstance* EquipInstance = QuickSlotEntries[QuickSlotIndex].EquipmentInstance;
		if (EquipInstance)
		{
			EquipInstance->OnEquipped();
		}

		CurrentActiveSlotEntry = QuickSlotEntries[QuickSlotIndex];
		OnActiveSlotChanged.Broadcast(CurrentActiveSlotEntry);
	}

	EquipItem(CurrentActiveSlotEntry.ItemInstance);
}

void UOvrlEquipmentManagerComponent::UnequipItemInSlot()
{
	if (CurrentActiveSlotEntry.EquipmentInstance)
	{
		CurrentActiveSlotEntry.EquipmentInstance->OnUnequipped();
	}

	UnequipItem(CurrentActiveSlotEntry.ItemInstance);
	CurrentActiveSlotEntry.Invalidate();
}

void UOvrlEquipmentManagerComponent::UnequipItem(UOvrlItemInstance* ItemToUnequip)
{
	if (!ItemToUnequip)
	{
		return;
	}

	EquippedItems.Remove(ItemToUnequip);

	if (UOvrlAbilitySystemComponent* ASC = GetOwnerAbilitySystemComponent())
	{
		// When unequip the item, remove all given abilities/effects/attributes from owner's ASC
		ItemToUnequip->GetGrantedHandles().TakeFromAbilitySystem(ASC);
	}

	OnItemUnequipped.Broadcast(ItemToUnequip);
}

int32 UOvrlEquipmentManagerComponent::AddItemToQuickSlots(UOvrlItemInstance* Item)
{
	int32 SlotIndex = GetFirstAvailableQuickSlotIndex();
	if (SlotIndex == INDEX_NONE)
	{
		SlotIndex = QuickSlotIndex;
	}

	FQuickSlotEntry SlotEntry;
	SlotEntry.ItemInstance = Item;
	QuickSlotEntries[SlotIndex] = SlotEntry;

	return SlotIndex;
}

void UOvrlEquipmentManagerComponent::OnInventoryItemRemoved(UOvrlItemInstance* RemovedItem)
{
	for (auto It = EquippedItems.CreateIterator(); It; ++It)
	{
		UOvrlItemInstance*& ItemInstance = *It;

		if (ItemInstance == RemovedItem)
		{
			RemoveItemFromQuickSlots(RemovedItem);
			UnequipItem(RemovedItem);
			return;
		}
	}
}

void UOvrlEquipmentManagerComponent::RemoveItemFromQuickSlots(UOvrlItemInstance* Item)
{
	for (auto It = QuickSlotEntries.CreateIterator(); It; ++It)
	{
		FQuickSlotEntry& QuickSlotEntry = *It;
		if (QuickSlotEntry.ItemInstance == Item)
		{
			if (QuickSlotEntry.EquipmentInstance)
			{
				QuickSlotEntry.EquipmentInstance->OnBeforeUnequip();
				QuickSlotEntry.EquipmentInstance->Destroy();
			}

			// Check if the item removed is the same we have currently selected
			if (GetActiveSlotItemInstance() == Item)
			{
				CurrentActiveSlotEntry.Invalidate();
			}

			QuickSlotEntry.Invalidate();
			OnActiveSlotChanged.Broadcast(QuickSlotEntry);
			return;
		}
	}
}

int32 UOvrlEquipmentManagerComponent::GetFirstAvailableQuickSlotIndex() const
{
	for (int32 i = 0; i < NumQuickSlots; i++)
	{
		if (!QuickSlotEntries[i].ItemInstance)
		{
			return i;
		}
	}

	return INDEX_NONE;
}

const FQuickSlotEntry& UOvrlEquipmentManagerComponent::FindFirstQuickSlotEntryByDefinition(TSubclassOf<UOvrlItemDefinition> ItemDefinition) const
{
	for (const FQuickSlotEntry& QuickSlotEntry : QuickSlotEntries)
	{
		if (QuickSlotEntry.EquipmentInstance && QuickSlotEntry.EquipmentInstance->GetAssociatedItem()->GetItemDefClass() == ItemDefinition)
		{
			return QuickSlotEntry;
		}
	}

	static FQuickSlotEntry EMPTY_SLOT;
	return EMPTY_SLOT;
}

UOvrlAbilitySystemComponent* UOvrlEquipmentManagerComponent::GetOwnerAbilitySystemComponent() const
{
	return Cast<UOvrlAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()));
}

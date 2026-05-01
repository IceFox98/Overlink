#include "Inventory/OvrlInventoryComponent.h"

#include "Inventory/OvrlItemInstance.h"
#include "Inventory/OvrlItemFragment_EquippableItem.h"
#include "Inventory/OvrlItemFragment_SetStats.h"
#include "Inventory/OvrlItemPickupActor.h"
#include "Equipment/OvrlEquipmentInstance.h"
#include "Equipment/OvrlEquipmentDefinition.h"

#include "AbilitySystem/OvrlAbilitySystemComponent.h"
#include "AbilitySystem/OvrlAbilitySet.h"

#include "Kismet/GameplayStatics.h"
#include "AbilitySystemGlobals.h"
#include "OvrlLogUtils.h"

UOvrlInventoryComponent::UOvrlInventoryComponent()
{
	QuickSlotIndex = -1;
}

void UOvrlInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	for (const FOvrlInitialItemData& InitialItem : InitialItems)
	{
		AddItemFromDefinition(InitialItem.ItemDefinition, InitialItem.PickupClass, InitialItem.Count);
	}
}

UOvrlItemInstance* UOvrlInventoryComponent::AddItemFromDefinition(TSubclassOf<UOvrlItemDefinition> ItemDefinition, TSubclassOf<AOvrlItemPickupActor> PickupClass, int32 Count/* = 1*/)
{
	if (!ItemDefinition)
	{
		OVRL_LOG_ERR(LogOverlink, true, "ItemDefinition is NULL!");
		return nullptr;
	}

	UOvrlItemInstance* ItemInstance = nullptr;

	ItemInstance = NewObject<UOvrlItemInstance>(GetOwner());
	ItemInstance->SetItemDef(ItemDefinition);
	ItemInstance->PickupClass = PickupClass; // Set pickup class so we know what class to use when drop the item

	// Instantiate the item fragments
	for (const UOvrlItemFragment* Fragment : GetDefault<UOvrlItemDefinition>(ItemDefinition)->Fragments)
	{
		if (Fragment)
		{
			Fragment->OnInstanceCreated(ItemInstance);
		}
	}

	AddItem(ItemInstance, Count);

	return ItemInstance;
}

void UOvrlInventoryComponent::AddItem(UOvrlItemInstance* Item, int32 Count/* = 1*/)
{
	if (!Item)
	{
		OVRL_LOG_ERR(LogOverlink, true, "Item is NULL!");
		return;
	}

	FOvrlItemEntry ItemEntry;
	ItemEntry.Instance = Item;
	ItemEntry.Count = FMath::Max(1, Count); // Must be at least 1
	ItemEntries.Add(ItemEntry);

	// Spawn item if we find an equippable fragment
	if (const UOvrlItemFragment_EquippableItem* EquipInfo = Item->FindFragmentByClass<UOvrlItemFragment_EquippableItem>())
	{
		TSubclassOf<UOvrlEquipmentDefinition> EquipDefClass = EquipInfo->EquipmentDefinition;
		if (EquipDefClass)
		{
			const UOvrlEquipmentDefinition* EquipmentDef = GetDefault<UOvrlEquipmentDefinition>(EquipDefClass);

			if (EquipmentDef->bShouldSpawnEquipmentInstance)
			{
				if (EquipmentDef->EquipmentClass)
				{
					FActorSpawnParameters SpawnParams;
					SpawnParams.Owner = GetOwner();
					SpawnParams.Instigator = Cast<APawn>(GetOwner());

					AOvrlEquipmentInstance* EquipmentInstance = GetWorld()->SpawnActor<AOvrlEquipmentInstance>(EquipmentDef->EquipmentClass, SpawnParams);
					EquipmentInstance->Initialize(EquipDefClass, Item);

					EquippedInstances.Emplace(EquipmentInstance);

					SetActiveSlotIndex(EquippedInstances.Num() - 1);
				}
				else
				{
					OVRL_LOG_WARN(LogOverlink, true, "Tried to spawn equipment instance, but EquipmentClass was not valid! Asset: %s", *EquipDefClass->GetName());
				}
			}
		}
	}

	OnItemUpdated.Broadcast(ItemEntry, true);
}

UOvrlAbilitySystemComponent* UOvrlInventoryComponent::GetAbilitySystemComponent() const
{
	return Cast<UOvrlAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()));
}

void UOvrlInventoryComponent::SetActiveSlotIndex(int32 NewIndex, bool bForceSet /* = false*/)
{
	if (!bForceSet)
	{
		if (QuickSlotIndex == NewIndex || TimerHandle_EquipItem.IsValid())
		{
			// Do nothing if index is the same.
			return;
		}
	}

	if (EquippedInstances.IsValidIndex(NewIndex))
	{
		AOvrlEquipmentInstance* NewEquipInstance = EquippedInstances[NewIndex];
		if (ensure(NewEquipInstance))
		{
			if (CurrentEquippedInstance)
			{
				CurrentEquippedInstance->OnBeforeUnequip(); // Prepare current item to unequip (stop firing/animations...)
			}

			const float EquipNotifyTime = NewEquipInstance->GetEquipNotifyTime();
			NewEquipInstance->PlayEquipMontage();

			if (EquipNotifyTime > 0.f)
			{
				// Simulate weapon switch animation, but actually perform the switch only after specific amount of time
				FTimerDelegate TimerDelegate;
				TimerDelegate.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(UOvrlInventoryComponent, SetActiveSlotIndex_Internal), NewIndex);
				GetWorld()->GetTimerManager().SetTimer(TimerHandle_EquipItem, TimerDelegate, EquipNotifyTime, false);
			}
			else
			{
				// Instant item switch
				SetActiveSlotIndex_Internal(NewIndex);
			}
		}
	}
}

FOvrlItemEntry UOvrlInventoryComponent::FindFirstItemEntryByDefinition(TSubclassOf<UOvrlItemDefinition> ItemDefinition) const
{
	for (const FOvrlItemEntry& ItemEntry : ItemEntries)
	{
		if (ItemEntry.Instance && ItemEntry.Instance->GetItemDefClass() == ItemDefinition)
		{
			return ItemEntry;
		}
	}

	return FOvrlItemEntry();
}

AOvrlEquipmentInstance* UOvrlInventoryComponent::FindFirstEquipmentInstanceByDefinition(TSubclassOf<UOvrlItemDefinition> ItemDefinition) const
{
	for (AOvrlEquipmentInstance* EquipmentInstance : EquippedInstances)
	{
		if (EquipmentInstance && EquipmentInstance->GetAssociatedItem()->GetItemDefClass() == ItemDefinition)
		{
			return EquipmentInstance;
		}
	}

	return nullptr;
}

void UOvrlInventoryComponent::AddItemCount(UOvrlItemInstance* Item, int32 CountToAdd, bool bCreateItemIfMissing)
{
	for (FOvrlItemEntry& ItemEntry : ItemEntries)
	{
		if (ItemEntry.Instance == Item)
		{
			ItemEntry.Count += FMath::Max(0, CountToAdd);
			OnItemUpdated.Broadcast(ItemEntry, false);
			return;
		}
	}

	// Didn't find the item, let's create it if needed
	if (bCreateItemIfMissing)
	{
		AddItem(Item, CountToAdd);
	}
}

void UOvrlInventoryComponent::AddItemCountByDefinition(TSubclassOf<UOvrlItemDefinition> ItemDefinition, int32 CountToAdd,
                                                       bool bCreateItemIfMissing, const TSubclassOf<AOvrlItemPickupActor>& PickupClass)
{
	for (FOvrlItemEntry& ItemEntry : ItemEntries)
	{
		if (ItemEntry.Instance && ItemEntry.Instance->GetItemDefClass() == ItemDefinition)
		{
			ItemEntry.Count += FMath::Max(0, CountToAdd);
			OnItemUpdated.Broadcast(ItemEntry, false);
			return;
		}
	}

	// Didn't find the item, let's create it if needed
	if (bCreateItemIfMissing)
	{
		AddItemFromDefinition(ItemDefinition, PickupClass, CountToAdd);
	}
}

void UOvrlInventoryComponent::SetActiveSlotIndex_Internal(int32 NewIndex)
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_EquipItem);

	UnequipItemInSlot();

	QuickSlotIndex = NewIndex;

	EquipItemInSlot();
}

void UOvrlInventoryComponent::EquipItemInSlot()
{
	// You can equip a new Item only if there's no current equipped item.
	// Be sure to call UnequipCurrentItem first
	if (!CurrentEquippedInstance && EquippedInstances.IsValidIndex(QuickSlotIndex))
	{
		AOvrlEquipmentInstance* EquipInstance = EquippedInstances[QuickSlotIndex];
		EquipInstance->OnEquipped();

		const UOvrlEquipmentDefinition* EquipmentDef = GetDefault<UOvrlEquipmentDefinition>(EquipInstance->EquipmentDefinitionClass);

		if (UOvrlAbilitySystemComponent* ASC = GetAbilitySystemComponent())
		{
			// When the item is equipped, we give all its abilities/effects/attributes to player's ASC
			for (TObjectPtr<const UOvrlAbilitySet> AbilitySet : EquipmentDef->AbilitySetsToGrant)
			{
				AbilitySet->GiveToAbilitySystem(ASC, /*inout*/ &EquipInstance->GrantedHandles, EquipInstance);
			}
		}

		CurrentEquippedInstance = EquipInstance;
		OnItemEquipped.Broadcast(CurrentEquippedInstance);
	}
}

void UOvrlInventoryComponent::UnequipItemInSlot()
{
	UnequipItem(CurrentEquippedInstance);
	CurrentEquippedInstance = nullptr;
}

void UOvrlInventoryComponent::UnequipItem(AOvrlEquipmentInstance* ItemToUnequip) const
{
	if (ItemToUnequip)
	{
		ItemToUnequip->OnUnequipped();

		if (UOvrlAbilitySystemComponent* ASC = GetAbilitySystemComponent())
		{
			// When unequip the item, remove all given abilities/effects/attributes from player's ASC
			ItemToUnequip->GrantedHandles.TakeFromAbilitySystem(ASC);
		}
	}
}

void UOvrlInventoryComponent::RemoveItem(UOvrlItemInstance* ItemToRemove, int32 Count/* = 1*/)
{
	bool bItemRemoved = false;
	for (auto EntryIt = ItemEntries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FOvrlItemEntry& Entry = *EntryIt;
		if (Entry.Instance == ItemToRemove)
		{
			// Decrease the item count
			Entry.Count -= FMath::Max(1, Count);

			// If equal/lower than 0, remove the item
			if (Entry.Count <= 0)
			{
				EntryIt.RemoveCurrent();
				bItemRemoved = true;
				break;
			}
		}
	}

	// Search if the item is also an equipment, in that case unequip it
	for (auto It = EquippedInstances.CreateIterator(); It; ++It)
	{
		AOvrlEquipmentInstance*& EquipInstance = *It;

		if (EquipInstance && EquipInstance->GetAssociatedItem() == ItemToRemove)
		{
			EquipInstance->OnBeforeUnequip();
			UnequipItem(EquipInstance);

			// If the item we want to remove is currently equipped, set it to null.
			if (EquipInstance == CurrentEquippedInstance)
			{
				CurrentEquippedInstance = nullptr;
			}

			EquipInstance->Destroy();

			// Remove from equipped item list
			It.RemoveCurrent();

			RefreshQuickSlot();
			break;
		}
	}

	if (bItemRemoved)
	{
		OnItemRemoved.Broadcast();
	}
}

void UOvrlInventoryComponent::RefreshQuickSlot()
{
	if (EquippedInstances.Num() > 0)
	{
		if (!EquippedInstances.IsValidIndex(QuickSlotIndex))
		{
			QuickSlotIndex -= 1;
		}

		SetActiveSlotIndex(QuickSlotIndex, true);
	}
	else
	{
		QuickSlotIndex = -1; // Set to -1 since we don't have any equipped items left
	}
}

void UOvrlInventoryComponent::DropItem(UOvrlItemInstance* ItemToDrop)
{
	if (!ItemToDrop)
	{
		return;
	}

	// Deferred spawn so we can set cached item before BeginPlay is called
	AOvrlItemPickupActor* ItemPickupActor = GetWorld()->SpawnActorDeferred<AOvrlItemPickupActor>(ItemToDrop->PickupClass, GetOwner()->GetActorTransform());
	ItemPickupActor->SetCachedItemInstance(ItemToDrop);
	UGameplayStatics::FinishSpawningActor(ItemPickupActor, GetOwner()->GetActorTransform());

	// Let pickup actor handle the drop logic
	ItemPickupActor->Drop();

	RemoveItem(ItemToDrop);
}

int32 UOvrlInventoryComponent::GetDefaultStatFromItemDef(const TSubclassOf<UOvrlItemDefinition> WeaponItemClass, FGameplayTag StatTag)
{
	if (WeaponItemClass)
	{
		if (UOvrlItemDefinition* WeaponItemCDO = WeaponItemClass->GetDefaultObject<UOvrlItemDefinition>())
		{
			if (const UOvrlItemFragment_SetStats* ItemStatsFragment = Cast<UOvrlItemFragment_SetStats>(WeaponItemCDO->FindFragmentByClass(UOvrlItemFragment_SetStats::StaticClass())))
			{
				return ItemStatsFragment->GetItemStatByTag(StatTag);
			}
		}
	}

	return 0;
}

#include "Inventory/OvrlInventoryComponent.h"

#include "Inventory/OvrlItemInstance.h"
#include "Inventory/OvrlItemFragment_EquippableItem.h"
#include "Inventory/OvrlItemFragment_SetStats.h"
#include "Inventory/OvrlItemPickupActor.h"
#include "Equipment/OvrlEquipmentInstance.h"
#include "Equipment/OvrlEquipmentDefinition.h"
#include "OvrlItemUtils.h"
#include "OvrlLogUtils.h"

#include "AbilitySystem/OvrlAbilitySystemComponent.h"
#include "AbilitySystem/OvrlAbilitySet.h"

#include "Kismet/GameplayStatics.h"
#include "AbilitySystemGlobals.h"
#include "Inventory/OvrlItemFragment_PickupableItem.h"
#include "Inventory/OvrlPickupDefinition.h"

UOvrlInventoryComponent::UOvrlInventoryComponent()
{
	QuickSlotIndex = -1;
}

void UOvrlInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	for (const FOvrlInitialItemData& InitialItem : InitialItems)
	{
		AddItemFromDefinition(InitialItem.ItemDefinition, InitialItem.Quantity);
	}
}

UOvrlItemInstance* UOvrlInventoryComponent::AddItemFromDefinition(TSubclassOf<UOvrlItemDefinition> ItemDefinition, int32 Quantity/* = 1*/)
{
	if (!ItemDefinition)
	{
		OVRL_LOG_ERR(LogOverlink, true, "ItemDefinition is NULL!");
		return nullptr;
	}

	UOvrlItemInstance* ItemInstance = NewObject<UOvrlItemInstance>(GetOwner(), ItemDefinition->GetFName());
	ItemInstance->SetItemDef(ItemDefinition);

	// Instantiate the item fragments
	for (const UOvrlItemFragment* Fragment : GetDefault<UOvrlItemDefinition>(ItemDefinition)->Fragments)
	{
		if (Fragment)
		{
			Fragment->OnInstanceCreated(ItemInstance);
		}
	}

	AddItem(ItemInstance, Quantity);

	return ItemInstance;
}

void UOvrlInventoryComponent::AddItem(UOvrlItemInstance* Item, int32 Quantity/* = 1*/)
{
	if (!Item)
	{
		OVRL_LOG_ERR(LogOverlink, true, "Item is NULL!");
		return;
	}

	FOvrlItemEntry ItemEntry;
	ItemEntry.Instance = Item;
	ItemEntry.Quantity = FMath::Max(1, Quantity); // Must be at least 1
	ItemEntries.Add(ItemEntry);

	// // Spawn item if we find an equippable fragment
	// if (const UOvrlItemFragment_EquippableItem* EquipInfo = Item->FindFragmentByClass<UOvrlItemFragment_EquippableItem>())
	// {
	// 	TSubclassOf<UOvrlEquipmentDefinition> EquipDefClass = EquipInfo->EquipmentDefinition;
	// 	if (EquipDefClass)
	// 	{
	// 		const UOvrlEquipmentDefinition* EquipmentDef = GetDefault<UOvrlEquipmentDefinition>(EquipDefClass);
	//
	// 		if (EquipmentDef->bShouldSpawnEquipmentInstance)
	// 		{
	// 			if (EquipmentDef->EquipmentClass)
	// 			{
	// 				FActorSpawnParameters SpawnParams;
	// 				SpawnParams.Owner = GetOwner();
	// 				SpawnParams.Instigator = Cast<APawn>(GetOwner());
	//
	// 				AOvrlEquipmentInstance* EquipmentInstance = GetWorld()->SpawnActor<AOvrlEquipmentInstance>(EquipmentDef->EquipmentClass, SpawnParams);
	// 				EquipmentInstance->Initialize(EquipDefClass, Item);
	//
	// 				EquippedInstances.Emplace(EquipmentInstance);
	//
	// 				SetActiveSlotIndex(EquippedInstances.Num() - 1);
	// 			}
	// 			else
	// 			{
	// 				OVRL_LOG_WARN(LogOverlink, true, "Tried to spawn equipment instance, but EquipmentClass was not valid! Asset: %s", *EquipDefClass->GetName());
	// 			}
	// 		}
	// 	}
	// }

	// Richiamare anche OnItemUpdated?
	OnItemAdded.Broadcast(Item);
	//OnItemUpdated.Broadcast(ItemEntry, true);
}

//
// UOvrlAbilitySystemComponent* UOvrlInventoryComponent::GetAbilitySystemComponent() const
// {
// 	return Cast<UOvrlAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()));
// }

void UOvrlInventoryComponent::SetActiveSlotIndex(int32 NewIndex, bool bForceSet /* = false*/)
{
	// if (!bForceSet)
	// {
	// 	if (QuickSlotIndex == NewIndex || TimerHandle_EquipItem.IsValid())
	// 	{
	// 		// Do nothing if index is the same.
	// 		return;
	// 	}
	// }
	//
	// if (EquippedInstances.IsValidIndex(NewIndex))
	// {
	// 	AOvrlEquipmentInstance* NewEquipInstance = EquippedInstances[NewIndex];
	// 	if (ensure(NewEquipInstance))
	// 	{
	// 		if (CurrentEquippedInstance)
	// 		{
	// 			CurrentEquippedInstance->OnBeforeUnequip(); // Prepare current item to unequip (stop firing/animations...)
	// 		}
	//
	// 		const float EquipNotifyTime = NewEquipInstance->GetEquipNotifyTime();
	// 		NewEquipInstance->PlayEquipMontage();
	//
	// 		if (EquipNotifyTime > 0.f)
	// 		{
	// 			// Simulate weapon switch animation, but actually perform the switch only after specific amount of time
	// 			FTimerDelegate TimerDelegate;
	// 			TimerDelegate.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(UOvrlInventoryComponent, SetActiveSlotIndex_Internal), NewIndex);
	// 			GetWorld()->GetTimerManager().SetTimer(TimerHandle_EquipItem, TimerDelegate, EquipNotifyTime, false);
	// 		}
	// 		else
	// 		{
	// 			// Instant item switch
	// 			SetActiveSlotIndex_Internal(NewIndex);
	// 		}
	// 	}
	// }
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

void UOvrlInventoryComponent::AddItemQuantity(UOvrlItemInstance* Item, int32 QuantityToAdd, bool bCreateItemIfMissing)
{
	for (FOvrlItemEntry& ItemEntry : ItemEntries)
	{
		if (ItemEntry.Instance == Item)
		{
			ItemEntry.Quantity += FMath::Max(0, QuantityToAdd);
			OnItemUpdated.Broadcast(ItemEntry, false);
			return;
		}
	}

	// Didn't find the item, let's create it if needed
	if (bCreateItemIfMissing)
	{
		AddItem(Item, QuantityToAdd);
	}
}

UOvrlItemInstance* UOvrlInventoryComponent::AddItemQuantityByDefinition(TSubclassOf<UOvrlItemDefinition> ItemDefinition, int32 QuantityToAdd, bool bCreateItemIfMissing)
{
	for (FOvrlItemEntry& ItemEntry : ItemEntries)
	{
		if (ItemEntry.Instance && ItemEntry.Instance->GetItemDefClass() == ItemDefinition)
		{
			ItemEntry.Quantity += FMath::Max(0, QuantityToAdd);
			OnItemUpdated.Broadcast(ItemEntry, false);
			return ItemEntry.Instance;
		}
	}

	// Didn't find the item, let's create it if needed
	if (bCreateItemIfMissing)
	{
		return AddItemFromDefinition(ItemDefinition, QuantityToAdd);
	}

	return nullptr;
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

		// const UOvrlEquipmentDefinition* EquipmentDef = GetDefault<UOvrlEquipmentDefinition>(EquipInstance->EquipmentDefinitionClass);
		//
		// if (UOvrlAbilitySystemComponent* ASC = GetAbilitySystemComponent())
		// {
		// 	// When the item is equipped, we give all its abilities/effects/attributes to player's ASC
		// 	for (TObjectPtr<const UOvrlAbilitySet> AbilitySet : EquipmentDef->AbilitySetsToGrant)
		// 	{
		// 		AbilitySet->GiveToAbilitySystem(ASC, /*inout*/ &EquipInstance->GrantedHandles, EquipInstance);
		// 	}
		// }

		CurrentEquippedInstance = EquipInstance;
		// OnItemEquipped.Broadcast(CurrentEquippedInstance);
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

		// if (UOvrlAbilitySystemComponent* ASC = GetAbilitySystemComponent())
		// {
		// 	// When unequip the item, remove all given abilities/effects/attributes from player's ASC
		// 	ItemToUnequip->GrantedHandles.TakeFromAbilitySystem(ASC);
		// }
	}
}

void UOvrlInventoryComponent::RemoveItem(UOvrlItemInstance* ItemToRemove, int32 Quantity/* = 1*/)
{
	bool bItemRemoved = false;
	for (auto EntryIt = ItemEntries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FOvrlItemEntry& Entry = *EntryIt;
		if (Entry.Instance == ItemToRemove)
		{
			// Decrease the item quantity
			Entry.Quantity -= FMath::Max(1, Quantity);

			// If equal/lower than 0, remove the item
			if (Entry.Quantity <= 0)
			{
				EntryIt.RemoveCurrent();
				bItemRemoved = true;
				break; // Exit immediately, don't fire updated event
			}

			OnItemUpdated.Broadcast(Entry, false);
		}
	}

	// // Search if the item is also an equipment, in that case unequip it
	// for (auto It = EquippedInstances.CreateIterator(); It; ++It)
	// {
	// 	AOvrlEquipmentInstance*& EquipInstance = *It;
	//
	// 	if (EquipInstance && EquipInstance->GetAssociatedItem() == ItemToRemove)
	// 	{
	// 		EquipInstance->OnBeforeUnequip();
	// 		UnequipItem(EquipInstance);
	//
	// 		// If the item we want to remove is currently equipped, set it to null.
	// 		if (EquipInstance == CurrentEquippedInstance)
	// 		{
	// 			CurrentEquippedInstance = nullptr;
	// 		}
	//
	// 		EquipInstance->Destroy();
	//
	// 		// Remove from equipped item list
	// 		It.RemoveCurrent();
	//
	// 		RefreshQuickSlot();
	// 		break;
	// 	}
	// }

	if (bItemRemoved)
	{
		OnItemRemoved.Broadcast(ItemToRemove);
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

void UOvrlInventoryComponent::DropItem(UOvrlItemInstance* ItemToDrop, int32 Quantity/* = 1*/)
{
	const UOvrlPickupDefinition* PickupDefinition = UOvrlItemUtils::GetPickupDefinitionFromItemInstance(ItemToDrop);
	
	if (PickupDefinition)
	{
		// Deferred spawn so we can set cached item before BeginPlay is called
		AOvrlItemPickupActor* ItemPickupActor = GetWorld()->SpawnActorDeferred<AOvrlItemPickupActor>(PickupDefinition->BasePickupClass, GetOwner()->GetActorTransform());
		if (!ItemPickupActor)
		{
			OVRL_LOG_ERR(LogOverlink, true, "Failed to spawn the Pickup Actor of the item '%s'. Check if the BasePickupClass of '%s' is set", *ItemToDrop->GetName(), *PickupDefinition->GetName());
			return;
		}
		
		ItemPickupActor->SetCachedItemInstance(ItemToDrop, Quantity);
		UGameplayStatics::FinishSpawningActor(ItemPickupActor, GetOwner()->GetActorTransform());

		// Let pickup actor handle the drop logic
		ItemPickupActor->Drop(); 
	
		OnItemDropped.Broadcast(ItemToDrop);

		RemoveItem(ItemToDrop);
	}
	else
	{
		OVRL_LOG_WARN(LogOverlink, true, "Failed to drop the item '%s'.", *ItemToDrop->GetName());
	}
}
#include "Inventory/OvrlInventoryComponent.h"

// Internal
#include "Inventory/OvrlItemInstance.h"
#include "Inventory/OvrlItemFragment_SetStats.h"
#include "Inventory/OvrlItemPickupActor.h"
#include "Inventory/OvrlPickupDefinition.h"
#include "SaveSystem/OvrlSaveTypes.h"
#include "OvrlItemUtils.h"
#include "OvrlLogUtils.h"

// Engine
#include "Kismet/GameplayStatics.h"

void UOvrlInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	for (const FOvrlInitialItemData& InitialItem : InitialItems)
	{
		AddItemFromDefinition(InitialItem.ItemDefinition, InitialItem.Quantity);
	}

	for (FInventoryItemEntrySaveData InventoryEntry : SavedItems)
	{
		UOvrlItemInstance* ItemInstance = CreateUniqueItem(InventoryEntry.ItemDefinition, InventoryEntry.ItemGuid);
		if (ItemInstance)
		{
			ItemInstance->ReplaceStacks(InventoryEntry.Stacks); // Override any existing item stacks
			AddItem(ItemInstance, InventoryEntry.Quantity);
		}
	}
}

UOvrlItemInstance* UOvrlInventoryComponent::AddItemFromDefinition(TSubclassOf<UOvrlItemDefinition> ItemDefinition, int32 Quantity/* = 1*/)
{
	UOvrlItemInstance* ItemInstance = CreateUniqueItem(ItemDefinition);

	AddItem(ItemInstance, Quantity);

	return ItemInstance;
}

UOvrlItemInstance* UOvrlInventoryComponent::CreateUniqueItem(TSubclassOf<UOvrlItemDefinition> ItemDefinition, FGuid ItemGuid /*= FGuid::NewGuid()*/) const
{
	if (!ItemDefinition)
	{
		OVRL_LOG_ERR(LogOverlink, true, "ItemDefinition is NULL!");
		return nullptr;
	}

	const FName ItemName = MakeUniqueObjectName(GetOwner(), UOvrlItemInstance::StaticClass(), ItemDefinition->GetFName());
	UOvrlItemInstance* ItemInstance = NewObject<UOvrlItemInstance>(GetOwner(), ItemName);
	ItemInstance->SetItemDef(ItemDefinition);
	ItemInstance->Guid = ItemGuid;

	// Instantiate the item fragments
	for (const UOvrlItemFragment* Fragment : GetDefault<UOvrlItemDefinition>(ItemDefinition)->Fragments)
	{
		if (Fragment)
		{
			Fragment->OnInstanceCreated(ItemInstance);
		}
	}

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
	OnItemAdded.Broadcast(Item);
}

void UOvrlInventoryComponent::AddItemQuantity(UOvrlItemInstance* Item, int32 QuantityToAdd, bool bCreateItemIfMissing)
{
	for (FOvrlItemEntry& ItemEntry : ItemEntries)
	{
		if (ItemEntry.Instance == Item)
		{
			ItemEntry.Quantity += FMath::Max(0, QuantityToAdd);
			OnItemUpdated.Broadcast(ItemEntry);
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
			OnItemUpdated.Broadcast(ItemEntry);
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

			OnItemUpdated.Broadcast(Entry);
		}
	}

	if (bItemRemoved)
	{
		OnItemRemoved.Broadcast(ItemToRemove);
	}
}

void UOvrlInventoryComponent::DropItem(UOvrlItemInstance* ItemToDrop, int32 Quantity/* = 1*/)
{
	if (!ItemToDrop)
	{
		OVRL_LOG_WARN(LogOverlink, true, "ItemToDrop is NULL!");
		return;
	}

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

		const FOvrlItemEntry ItemEntry = FindFirstItemEntryByInstance(ItemToDrop);
		if (ItemEntry.Quantity > 0)
		{
			// Drop quantity should not exceed the actual quantity of the item.
			Quantity = FMath::Min(Quantity, ItemEntry.Quantity);
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

FOvrlItemEntry UOvrlInventoryComponent::FindFirstItemEntryByInstance(UOvrlItemInstance* Item) const
{
	for (const FOvrlItemEntry& ItemEntry : ItemEntries)
	{
		if (ItemEntry.Instance == Item)
		{
			return ItemEntry;
		}
	}

	return FOvrlItemEntry();
}

void UOvrlInventoryComponent::OnPreSave_Implementation()
{
	SavedItems.Empty();

	for (const FOvrlItemEntry& ItemEntry : GetItemEntries())
	{
		FInventoryItemEntrySaveData SaveData;
		SaveData.ItemGuid = ItemEntry.Instance->Guid;
		SaveData.ItemDefinition = ItemEntry.Instance->GetItemDefClass();
		SaveData.Stacks = ItemEntry.Instance->GetStacks();
		SaveData.Quantity = ItemEntry.Quantity;
		SavedItems.Add(SaveData);
	}
}

void UOvrlInventoryComponent::OnLoad_Implementation()
{
	InitialItems.Empty();
}

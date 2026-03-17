#include "Inventory/OvrlInventoryComponent.h"

#include "Inventory/OvrlItemInstance.h"
#include "Inventory/OvrlItemDefinition.h"
#include "Inventory/OvrlPickupDefinition.h"
#include "Inventory/OvrlItemFragment_EquippableItem.h"
#include "Inventory/OvrlItemFragment_SetStats.h"
#include "Inventory/OvrlItemPickupActor.h"
#include "Equipment/OvrlEquipmentInstance.h"
#include "Equipment/OvrlEquipmentDefinition.h"

#include "AbilitySystem/OvrlAbilitySystemComponent.h"
#include "AbilitySystem/OvrlAbilitySet.h"

#include "Kismet/GameplayStatics.h"
#include "AbilitySystemGlobals.h"
#include "Overlink.h"
#include "OvrlUtils.h"

UOvrlInventoryComponent::UOvrlInventoryComponent()
{
}

void UOvrlInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
	for (const auto& InitialItem : InitialItems)
	{
		AddItemFromDefinition(InitialItem.Key, InitialItem.Value);
	}
}

UOvrlItemInstance* UOvrlInventoryComponent::AddItemFromDefinition(TSubclassOf<UOvrlItemDefinition> ItemDefinition, int32 Count/* = 1*/)
{
	if (!ItemDefinition)
	{
		OVRL_LOG_ERR(LogOverlink, true, "ItemDefinition is NULL!");
		return nullptr;
	}

	UOvrlItemInstance* ItemInstance = nullptr;

	ItemInstance = NewObject<UOvrlItemInstance>(GetOwner());
	ItemInstance->SetItemDef(ItemDefinition);

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

			AOvrlEquipmentInstance* EquipmentInstance = GetWorld()->SpawnActor<AOvrlEquipmentInstance>(EquipmentDef->EquipmentClass);
			EquipmentInstance->EquipmentDefinitionClass = EquipDefClass;
			EquipmentInstance->AssociatedItem = Item;
			EquipmentInstance->SetOwner(GetOwner());
			EquipmentInstance->SetInstigator(Cast<APawn>(GetOwner()));
			EquipmentInstance->SetDisplayMesh(GetDefault<UOvrlItemDefinition>(Item->GetItemDef())->DisplayMesh);

			EquippedItems.Emplace(EquipmentInstance);
		}
	}

	SetActiveSlotIndex(EquippedItems.Num() - 1);
}

UOvrlAbilitySystemComponent* UOvrlInventoryComponent::GetAbilitySystemComponent() const
{
	return Cast<UOvrlAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()));
}

void UOvrlInventoryComponent::SetActiveSlotIndex(int32 NewIndex)
{
	if (EquippedItems.IsValidIndex(NewIndex))
	{
		AOvrlEquipmentInstance* NewEquipInstance = EquippedItems[NewIndex];
		if (ensure(NewEquipInstance))
		{
			const float EquipNotifyTime = NewEquipInstance->GetEquipNotifyTime();

			// Simulate weapon switch animation, but actually perform the switch only after specific amount of time
			FTimerDelegate TimerDelegate;
			TimerDelegate.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(UOvrlInventoryComponent, SetActiveSlotIndex_Internal), NewIndex);
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_EquipItem, TimerDelegate, EquipNotifyTime, false);

			NewEquipInstance->PlayEquipMontage();
		}
	}
}

FOvrlItemEntry UOvrlInventoryComponent::FindFirstItemEntryByDefinition(TSubclassOf<UOvrlItemDefinition> ItemDefinition) const
{
	for (const FOvrlItemEntry& ItemEntry : ItemEntries)
	{
		if (ItemEntry.Instance && ItemEntry.Instance->GetItemDef() == ItemDefinition)
		{
			return ItemEntry;
		}
	}

	return FOvrlItemEntry();
}

void UOvrlInventoryComponent::SetActiveSlotIndex_Internal(int32 NewIndex)
{
	UnequipItemInSlot();

	SelectedIndex = NewIndex;

	EquipItemInSlot();
}

void UOvrlInventoryComponent::EquipItemInSlot()
{
	// You can equip a new Item only if there's no current equipped item.
	// Be sure to call UnequipCurrentItem first
	if (!EquippedItem && EquippedItems.IsValidIndex(SelectedIndex))
	{
		AOvrlEquipmentInstance* EquipInstance = EquippedItems[SelectedIndex];
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

		EquippedItem = EquipInstance;
		OnItemEquipped.Broadcast(EquippedItem);
	}
}

void UOvrlInventoryComponent::UnequipItemInSlot()
{
	UnequipItem(EquippedItem);
	EquippedItem = nullptr;
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

		//ItemToUnequip = nullptr;
	}
}

void UOvrlInventoryComponent::RemoveItem(UOvrlItemInstance* ItemToRemove, int32 Count/* = 1*/)
{
	for (auto EntryIt = ItemEntries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FOvrlItemEntry& Entry = *EntryIt;
		if (Entry.Instance == ItemToRemove)
		{
			Entry.Count -= FMath::Max(1, Count);
			if (Entry.Count <= 0)
			{
				EntryIt.RemoveCurrent();
			}
		}
	}

	// Search if the item is also an equipment, in that case unequip it
	for (auto It = EquippedItems.CreateIterator(); It; ++It)
	{
		AOvrlEquipmentInstance*& EquipInstance = *It;

		if (EquipInstance && EquipInstance->GetAssociatedItem() == ItemToRemove)
		{
			UnequipItem(EquipInstance);
			EquipInstance->Destroy();

			It.RemoveCurrent();
			// TODO: Set EquippedItem = nullptr?
		}
	}
}

void UOvrlInventoryComponent::DropItem(UOvrlItemInstance* ItemToDrop)
{
	if (!ItemToDrop)
	{
		return;
	}

	FTransform Offset;
	Offset.SetLocation(FVector(300.f, 300.f, 20.f));
	Offset.SetScale3D(FVector::ZeroVector);

	AOvrlItemPickupActor* ItemPickupActor = GetWorld()->SpawnActorDeferred<AOvrlItemPickupActor>(AOvrlItemPickupActor::StaticClass(), GetOwner()->GetActorTransform() + Offset);
	ItemPickupActor->SetCachedItemInstance(ItemToDrop);

	UGameplayStatics::FinishSpawningActor(ItemPickupActor, GetOwner()->GetActorTransform() + Offset);

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

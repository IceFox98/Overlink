
#include "Inventory/OvrlInventoryComponent.h"

#include "Inventory/OvrlItemInstance.h"
#include "Inventory/OvrlItemDefinition.h"
#include "Inventory/OvrlPickupDefinition.h"
#include "Inventory/OvrlItemFragment_EquippableItem.h"
#include "Inventory/OvrlItemPickupActor.h"
#include "Equipment/OvrlEquipmentInstance.h"
#include "Equipment/OvrlEquipmentDefinition.h"

#include "AbilitySystem/OvrlAbilitySystemComponent.h"
#include "AbilitySystem/OvrlAbilitySet.h"

#include "Kismet/GameplayStatics.h"
#include "AbilitySystemGlobals.h"

UOvrlInventoryComponent::UOvrlInventoryComponent()
{
}

UOvrlItemInstance* UOvrlInventoryComponent::AddItemFromDefinition(TSubclassOf<UOvrlItemDefinition> ItemDef, int32 StackCount/* = 1*/)
{
	UOvrlItemInstance* ItemInstance = nullptr;

	ItemInstance = NewObject<UOvrlItemInstance>(GetOwner());
	ItemInstance->SetItemDef(ItemDef);

	// Instantiate the item fragments
	for (UOvrlItemFragment* Fragment : GetDefault<UOvrlItemDefinition>(ItemDef)->Fragments)
	{
		if (Fragment)
		{
			Fragment->OnInstanceCreated(ItemInstance);
		}
	}

	AddItem(ItemInstance, StackCount);

	return ItemInstance;
}

void UOvrlInventoryComponent::AddItem(UOvrlItemInstance* Item, int32 StackCount)
{
	Items.Add(Item);

	// Spawn Item if equippable
	if (const UOvrlItemFragment_EquippableItem* EquipInfo = Item->FindFragmentByClass<UOvrlItemFragment_EquippableItem>())
	{
		TSubclassOf<UOvrlEquipmentDefinition> EquipDefClass = EquipInfo->EquipmentDefinition;
		if (EquipDefClass)
		{
			const UOvrlEquipmentDefinition* EquipmentDef = GetDefault<UOvrlEquipmentDefinition>(EquipDefClass);

			AOvrlEquipmentInstance* EquipmentInstance = GetWorld()->SpawnActor<AOvrlEquipmentInstance>(EquipmentDef->InstanceType);
			EquipmentInstance->EquipmentDefinitionClass = EquipDefClass;
			EquipmentInstance->AssociatedItem = Item;
			EquipmentInstance->SetOwner(GetOwner());
			EquipmentInstance->SetInstigator(Cast<APawn>(GetOwner()));

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
//
//void UOvrlInventoryComponent::EquipItem(AOvrlEquipmentInstance* ItemToEquip)
//{
//	// You can equip a new Item only if there's no current equipped item.
//	// Be sure to call UnequipCurrentItem first
//	if (!EquippedItem && EquippedItems.IsValidIndex(SelectedIndex))
//	{
//		AOvrlEquipmentInstance* EquipInstance = EquippedItems[SelectedIndex];
//		EquipInstance->OnEquipped();
//
//		const UOvrlEquipmentDefinition* EquipmentDef = GetDefault<UOvrlEquipmentDefinition>(EquipInstance->EquipmentDefinitionClass);
//
//		if (UOvrlAbilitySystemComponent* ASC = GetAbilitySystemComponent())
//		{
//			// When the item is equipped, we give all its abilities/effects/attributes to player's ASC
//			for (TObjectPtr<const UOvrlAbilitySet> AbilitySet : EquipmentDef->AbilitySetsToGrant)
//			{
//				AbilitySet->GiveToAbilitySystem(ASC, /*inout*/ &EquipInstance->GrantedHandles, EquipInstance);
//			}
//		}
//
//		EquippedItem = EquipInstance;
//		OnItemEquipped.Broadcast(EquippedItem);
//	}
//}

void UOvrlInventoryComponent::UnequipItemInSlot()
{
	//if (EquippedItem)
	//{
	//	EquippedItem->OnUnequipped();

	//	if (UOvrlAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	//	{
	//		// When unequip the item, remove all given abilities/effects/attributes from player's ASC
	//		EquippedItem->GrantedHandles.TakeFromAbilitySystem(ASC);
	//	}

	//	EquippedItem = nullptr;
	//}

	UnequipItem(EquippedItem);
	EquippedItem = nullptr;
}

void UOvrlInventoryComponent::UnequipItem(AOvrlEquipmentInstance* ItemToUnequip)
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

//void UOvrlInventoryComponent::RemoveCurrentItem()
//{
//	RemoveItem(SelectedIndex);
//}

void UOvrlInventoryComponent::RemoveItem(UOvrlItemInstance* ItemToRemove)
{
	Items.Remove(ItemToRemove);

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
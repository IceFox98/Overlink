//// Fill out your copyright notice in the Description page of Project Settings.
//
//
//#include "Player/Components/InventoryComponent.h"
//#include "Items/EquippableItem.h"
//#include "Equipment/PuzzleGuyEquipmentInstance.h"
//#include "Player/PlayerCharacter.h"
//#include "PuzzleGuyUtils.h"
//#include "Items/PuzzleGuyPickupDefinition.h"
//#include "AbilitySystemGlobals.h"
//#include "AbilitySystem/PuzzleGuyAbilitySystemComponent.h"
//#include "AbilitySystem/PuzzleGuyAbilitySet.h"
//
//// Sets default values for this component's properties
//UInventoryComponent::UInventoryComponent()
//{
//	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
//	// off to improve performance if you don't need them.
//	PrimaryComponentTick.bCanEverTick = true;
//
//	SelectedIndex = -1;
//	SelectedItem = nullptr;
//}
//
//// Called when the game starts
//void UInventoryComponent::BeginPlay()
//{
//	Super::BeginPlay();
//
//}
//
//// Called every frame
//void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//}
//
//UItem* UInventoryComponent::AddItemFromPickup(UPuzzleGuyPickupDefinition* PickupDef)
//{
//	if (!PickupDef)
//		return nullptr;
//
//	UItem* NewEntry = NewObject<UItem>(GetOwner(), PickupDef->InventoryItemDefinition);
//	Items.Add(NewEntry);
//
//	if (NewEntry->IsA(UEquippableItem::StaticClass()))
//	{
//		UEquippableItem* ItemToEquip = Cast<UEquippableItem>(NewEntry);
//
//		// Spawn the Equippable Instance of the Item
//		AddEquippableItem(ItemToEquip, PickupDef);
//
//		SetActiveSlotIndex(EquippedItems.Num() - 1);
//	}
//
//	return NewEntry;
//}
//
//void UInventoryComponent::RemoveItem(int32 IndexToRemove)
//{
//	if (!Items.IsValidIndex(IndexToRemove)) 
//		return;
//
//	if (!EquippedItems.IsValidIndex(IndexToRemove))
//		return;
//
//	Items.RemoveAt(IndexToRemove);
//	EquippedItems.RemoveAt(IndexToRemove);
//}
//
//void UInventoryComponent::AddEquippableItem(UEquippableItem* NewItem, UPuzzleGuyPickupDefinition* PickupDef)
//{
//	if (NewItem && NewItem->InstanceType)
//	{
//		USceneComponent* ItemHoldingPoint = GetItemHoldingPoint();
//		USceneComponent* ItemAttachComp = GetItemAttachComp();
//
//		if (ItemHoldingPoint && ItemAttachComp)
//		{
//			APuzzleGuyEquipmentInstance* EquipInstance = GetWorld()->SpawnActor<APuzzleGuyEquipmentInstance>(NewItem->InstanceType);
//			EquipInstance->SetOwner(GetOwner());
//			EquipInstance->AttachToComponent(ItemAttachComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
//			EquipInstance->SetActorRelativeLocation(ItemHoldingPoint->GetRelativeLocation());
//			EquipInstance->EquipmentItem = NewItem;
//			EquipInstance->PickupDefinition = PickupDef;
//
//			EquippedItems.Add(EquipInstance);
//		}
//		else
//		{
//			CONSOLE_LOG(Warning, "UInventoryComponent::AddItem - ItemHoldingPoint not initialized");
//		}
//
//	}
//}
//
//void UInventoryComponent::EquipItemInSlot()
//{
//	// You can equip a new Item only if there's no current equipped item.
//	// Be sure to call UnequipCurrentItem first
//	if (!SelectedItem && EquippedItems.IsValidIndex(SelectedIndex))
//	{
//		APuzzleGuyEquipmentInstance* EquipInstance = EquippedItems[SelectedIndex];
//		EquipInstance->OnEquipped();
//
//		if (UPuzzleGuyAbilitySystemComponent* ASC = GetAbilitySystemComponent())
//		{
//			// When the item is equipped, we give all its abilities/effects/attributes to player's ASC
//			for (TObjectPtr<const UPuzzleGuyAbilitySet> AbilitySet : EquipInstance->EquipmentItem->AbilitySetsToGrant)
//			{
//				AbilitySet->GiveToAbilitySystem(ASC, /*inout*/ &EquipInstance->GrantedHandles, EquipInstance);
//			}
//		}
//
//		SelectedItem = EquipInstance;
//	}
//}
//
//void UInventoryComponent::UnequipCurrentItem()
//{
//	if (SelectedItem)
//	{
//		SelectedItem->OnUnequipped();
//
//		if (UPuzzleGuyAbilitySystemComponent* ASC = GetAbilitySystemComponent())
//		{
//			// When unequip the item, remove all given abilities/effects/attributes from player's ASC
//			SelectedItem->GrantedHandles.TakeFromAbilitySystem(ASC);
//		}
//
//		SelectedItem = nullptr;
//	}
//}
//
//void UInventoryComponent::SetActiveSlotIndex(int32 NewIndex)
//{
//	UnequipCurrentItem();
//
//	SelectedIndex = NewIndex;
//
//	EquipItemInSlot();
//}
//
//USceneComponent* UInventoryComponent::GetItemHoldingPoint() const
//{
//	APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwner());
//
//	if (Player)
//	{
//		return Player->GetItemHoldingPoint();
//	}
//
//	return nullptr;
//}
//
//USceneComponent* UInventoryComponent::GetItemAttachComp() const
//{
//	APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwner());
//
//	if (Player)
//	{
//		return Player->GetCameraComp();
//	}
//
//	return nullptr;
//}
//
//UPuzzleGuyAbilitySystemComponent* UInventoryComponent::GetAbilitySystemComponent() const
//{
//	AActor* OwningActor = GetOwner();
//	return Cast<UPuzzleGuyAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor));
//}
//
//void UInventoryComponent::NextItem()
//{
//	if (Items.IsEmpty())
//	{
//		SetActiveSlotIndex(-1);
//		return;
//	}
//
//	// Select next item
//	int32 NewIndex = (SelectedIndex + 1) % Items.Num();
//	SetActiveSlotIndex(NewIndex);
//}
//
//void UInventoryComponent::PrevItem()
//{
//	if (Items.IsEmpty())
//	{
//		SetActiveSlotIndex(-1);
//		return;
//	}
//
//	// Select prev item
//	int32 NewIndex = (SelectedIndex - 1 + Items.Num()) % Items.Num();
//	SetActiveSlotIndex(NewIndex);
//}
//
//AActor* UInventoryComponent::DropSelectedItem()
//{
//	if (SelectedItem)
//	{
//		RemoveItem(SelectedIndex);
//		
//		AActor* DroppedActor = SelectedItem->OnDrop();
//		SelectedItem->Destroy();
//		
//		PrevItem();
//
//		return DroppedActor;
//	}
//
//	return nullptr;
//}

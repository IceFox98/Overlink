// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/OvrlItemPickupActor.h"

// Internal
#include "Inventory/OvrlInventoryComponent.h"
#include "Inventory/OvrlPickupDefinition.h"
#include "Inventory/OvrlItemInstance.h"
#include "Inventory/OvrlItemDefinition.h"
#include "Inventory/OvrlItemFragment_PickupableItem.h"
#include "OvrlLogUtils.h"
#include "OvrlItemUtils.h"

// Engine
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"

AOvrlItemPickupActor::AOvrlItemPickupActor()
{
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	SetRootComponent(ItemMesh);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ItemMesh->SetGenerateOverlapEvents(false);

	PickupCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("PickupCollider"));
	PickupCollider->SetupAttachment(ItemMesh);
}

void AOvrlItemPickupActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	if (ItemDefinitionClass)
	{
		UOvrlItemDefinition* ItemDefinition = Cast<UOvrlItemDefinition>(ItemDefinitionClass->GetDefaultObject());
		ItemMesh->SetStaticMesh(ItemDefinition->DisplayMesh);
	}
}

void AOvrlItemPickupActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	static const FName NAME_ItemDefinitionClass = GET_MEMBER_NAME_CHECKED(AOvrlItemPickupActor, ItemDefinitionClass);
	const FName Name = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	// Update the data only when the ItemDefinition has changed
	if (Name == NAME_ItemDefinitionClass)
	{
		RefreshData();
	}
}

void AOvrlItemPickupActor::BeginPlay()
{
	Super::BeginPlay();

	PickupCollider->OnComponentBeginOverlap.AddDynamic(this, &AOvrlItemPickupActor::OnPickupColliderOverlap);

	RefreshData();
}

void AOvrlItemPickupActor::OnPickupColliderOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APawn* Pawn = Cast<APawn>(OtherActor))
	{
		HandlePickup(Pawn);
	}
}

void AOvrlItemPickupActor::HandlePickup(APawn* InInstigator)
{
	if (UOvrlInventoryComponent* InventoryComponent = InInstigator->GetComponentByClass<UOvrlInventoryComponent>())
	{
		// The cached item is valid only when the item was dropped from the player/enemy
		TSubclassOf<UOvrlItemDefinition> TargetItemDefinition = CachedItemInstance ? CachedItemInstance->GetItemDefClass() : ItemDefinitionClass;

		// Search for an item of the same type
		UOvrlItemInstance* ExistingItem = InventoryComponent->FindFirstItemEntryByDefinition(TargetItemDefinition).Instance;

		bool bHandled = true;
		if (ExistingItem) // Duplicated item found
		{
			if (bAddDuplicatedItem)
			{
				// Add it anyway
				AddItemToInventory(InventoryComponent);
			}
			else
			{
				// Let user manage the duplicated item
				bHandled = ManageDuplicatedItem(TargetItemDefinition, ExistingItem, InInstigator);
			}
		}
		else
		{
			// No duplicates found, execute default behavior
			AddItemToInventory(InventoryComponent);
		}

		// If the item has been handled successfully, we can destroy this Actor
		if (bHandled)
		{
			Destroy();
		}
	}
}

bool AOvrlItemPickupActor::ManageDuplicatedItem_Implementation(TSubclassOf<UOvrlItemDefinition> DuplicatedItemClass, UOvrlItemInstance* ExistingItem, APawn* ReceivingPawn)
{
	// By default, the item is considered successfully handled
	return true;
}

void AOvrlItemPickupActor::AddItemToInventory(UOvrlInventoryComponent* TargetInventoryComponent) const
{
	if (TargetInventoryComponent)
	{
		if (SavedItem.ItemGuid.IsValid())
		{
			TargetInventoryComponent->AddItemFromSaveData(SavedItem);
		}
		else if (HasCachedItem())
		{
			TargetInventoryComponent->AddItem(CachedItemInstance, Quantity);
		}
		else
		{
			TargetInventoryComponent->AddItemFromDefinition(ItemDefinitionClass, Quantity);
		}
	}
}

void AOvrlItemPickupActor::Drop_Implementation()
{
	// No base logic
}

UOvrlItemDefinition* AOvrlItemPickupActor::GetItemDefinition() const
{
	// If the item was dropped, this will be valid, so we can set the ItemDefinition that would be null otherwise.
	if (CachedItemInstance)
	{
		return CachedItemInstance->GetItemDef();
	}
	
	if (ItemDefinitionClass)
	{
		return Cast<UOvrlItemDefinition>(ItemDefinitionClass->GetDefaultObject());
	}
	
	return nullptr;
}

void AOvrlItemPickupActor::SetCachedItemInstance(UOvrlItemInstance* ItemToCache, int32 InQuantity/* = 1*/)
{
	CachedItemInstance = ItemToCache;
	Quantity = FMath::Max(1, InQuantity); // Must be at least 1
}

void AOvrlItemPickupActor::RefreshData() const
{
	if (const UOvrlItemDefinition* ItemDefinition = GetItemDefinition())
	{
		ItemMesh->SetStaticMesh(ItemDefinition->DisplayMesh);

		// If we find a pickupable fragment, set additional data.
		if (const UOvrlItemFragment_PickupableItem* PickupableItemFragment = ItemDefinition->FindFragmentByClass<UOvrlItemFragment_PickupableItem>())
		{
			if (const UOvrlPickupDefinition* PickupDefinition = PickupableItemFragment->PickupDefinition)
			{
				ItemMesh->SetRelativeScale3D(PickupDefinition->MeshScale);
				PickupCollider->SetRelativeTransform(PickupDefinition->PickupColliderTransform);
				PickupCollider->SetCapsuleHalfHeight(PickupDefinition->PickupColliderCapsuleHalfHeight);
				PickupCollider->SetCapsuleRadius(PickupDefinition->PickupColliderCapsuleRadius);
			}
		}
	}
}

void AOvrlItemPickupActor::OnPreSave_Implementation()
{
	if (HasCachedItem())
	{
		SavedItem.ItemGuid = CachedItemInstance->Guid;
		SavedItem.ItemDefinition = CachedItemInstance->GetItemDefClass();
		SavedItem.Stacks = CachedItemInstance->GetStacks();
		SavedItem.Quantity = Quantity;
	}
}

void AOvrlItemPickupActor::OnLoad_Implementation()
{
	if (SavedItem.ItemGuid.IsValid())
	{
		// Update item pickup info
		ItemDefinitionClass = SavedItem.ItemDefinition;
		Quantity = SavedItem.Quantity;
	}
}

#if WITH_EDITOR
void AOvrlItemPickupActor::UpdatePickupDefinition() const
{
	UOvrlPickupDefinition* PickupDefinition = UOvrlItemUtils::GetPickupDefinitionFromItemDefinition(ItemDefinitionClass);
	if (PickupDefinition)
	{
		PickupDefinition->MeshScale = ItemMesh->GetRelativeScale3D();
		PickupDefinition->PickupColliderTransform = PickupCollider->GetRelativeTransform();
		PickupDefinition->PickupColliderCapsuleHalfHeight = PickupCollider->GetUnscaledCapsuleHalfHeight();
		PickupDefinition->PickupColliderCapsuleRadius = PickupCollider->GetUnscaledCapsuleRadius();

		OVRL_LOG_INFO(LogOverlink, true, "Pickup Definition %s updated successfully!", *PickupDefinition->GetName());
	}
	else
	{
		OVRL_LOG_ERR(LogOverlink, true, "Failed to update Pickup Definition.");
	}
}
#endif

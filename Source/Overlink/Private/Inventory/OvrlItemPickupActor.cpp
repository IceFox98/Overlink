// Fill out your copyright notice in the Description page of Project Settings.

#include "Inventory/OvrlItemPickupActor.h"
#include "Inventory/OvrlInventoryComponent.h"
#include "Inventory/OvrlPickupDefinition.h"
#include "Inventory/OvrlItemInstance.h"
#include "Inventory/OvrlItemDefinition.h"

// Engine
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Inventory/OvrlItemFragment_PickupableItem.h"

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

	RefreshData();
}

void AOvrlItemPickupActor::BeginPlay()
{
	Super::BeginPlay();

	PickupCollider->OnComponentBeginOverlap.AddDynamic(this, &AOvrlItemPickupActor::OnPickupColliderOverlap);

	RefreshData();
}

void AOvrlItemPickupActor::Drop_Implementation()
{
	// No base logic
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

void AOvrlItemPickupActor::RefreshData()
{
	UOvrlItemDefinition* ItemDefinition = nullptr;

	// If the item was dropped, this will be valid, so we can set the ItemDefinition that would be null otherwise.
	if (CachedItemInstance)
	{
		ItemDefinition = CachedItemInstance->GetItemDef();
	}
	else if (ItemDefinitionClass)
	{
		ItemDefinition = Cast<UOvrlItemDefinition>(ItemDefinitionClass->GetDefaultObject());
	}

	if (ItemDefinition)
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

bool AOvrlItemPickupActor::ManageDuplicatedItem_Implementation(TSubclassOf<UOvrlItemDefinition> DuplicatedItemClass, UOvrlItemInstance* ExistingItem, APawn* ReceivingPawn)
{
	// By default, the item is considered successfully handled
	return true;
}

void AOvrlItemPickupActor::OnPickupColliderOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APawn* Pawn = Cast<APawn>(OtherActor))
	{
		HandlePickup(Pawn);
	}
}

void AOvrlItemPickupActor::AddItemToInventory(UOvrlInventoryComponent* TargetInventoryComponent)
{
	if (TargetInventoryComponent)
	{
		if (HasCachedItem())
		{
			TargetInventoryComponent->AddItem(CachedItemInstance);
		}
		else
		{
			TargetInventoryComponent->AddItemFromDefinition(ItemDefinitionClass);
		}
	}
}

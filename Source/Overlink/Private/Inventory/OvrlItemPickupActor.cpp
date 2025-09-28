// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/OvrlItemPickupActor.h"
#include "Inventory/OvrlInventoryComponent.h"
#include "Inventory/OvrlPickupDefinition.h"
#include "Inventory/OvrlItemInstance.h"
#include "Inventory/OvrlItemDefinition.h"

// Engine
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

AOvrlItemPickupActor::AOvrlItemPickupActor()
{
	PickupCollider = CreateDefaultSubobject<USphereComponent>(TEXT("PickupCollider"));
	SetRootComponent(PickupCollider);

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(PickupCollider);
}

void AOvrlItemPickupActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ShowItemMesh();
}

void AOvrlItemPickupActor::BeginPlay()
{
	Super::BeginPlay();

	PickupCollider->OnComponentBeginOverlap.AddDynamic(this, &AOvrlItemPickupActor::OnPickupColliderOverlap);

	ShowItemMesh();
}

void AOvrlItemPickupActor::ShowItemMesh()
{
	UOvrlItemDefinition* ItemDefinition = nullptr;

	// Pick the right mesh to display
	if (CachedItemInstance)
	{
		ItemDefinition = Cast<UOvrlItemDefinition>(CachedItemInstance->GetItemDef()->GetDefaultObject());
	}
	else if (ItemPickupDefinition)
	{
		ItemDefinition = Cast<UOvrlItemDefinition>(ItemPickupDefinition->ItemDefinition->GetDefaultObject());
	}

	if (ItemDefinition)
	{
		ItemMesh->SetStaticMesh(ItemDefinition->DisplayMesh);
	}
}

void AOvrlItemPickupActor::OnPickupColliderOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (const APawn* Pawn = Cast<APawn>(OtherActor))
	{
		if (UOvrlInventoryComponent* InventoryComponent = Pawn->GetComponentByClass<UOvrlInventoryComponent>())
		{
			// The cached item is valid only when the item was dropped from the player/enemy
			if (CachedItemInstance)
			{
				InventoryComponent->AddItem(CachedItemInstance, 1);
			}
			else
			{
				InventoryComponent->AddItemFromDefinition(ItemPickupDefinition->ItemDefinition, 1);
			}

			Destroy();
		}
	}
}

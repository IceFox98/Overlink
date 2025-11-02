// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/OvrlEquipmentInstance.h"
#include "Equipment/OvrlEquipmentDefinition.h"
#include "Animations/OvrlLinkedAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

#include "Player/OvrlCharacterBase.h"

// Sets default values
AOvrlEquipmentInstance::AOvrlEquipmentInstance()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AOvrlEquipmentInstance::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AOvrlEquipmentInstance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AOvrlEquipmentInstance::Destroyed()
{
	if (AOvrlCharacterBase* OwningPawn = Cast<AOvrlCharacterBase>(GetOwner()))
	{
		// Remove object "preview" from owner
		OwningPawn->UnequipObject();
		OwningPawn->RestoreAnimLayerClass();
	}

	Super::Destroyed();
}

void AOvrlEquipmentInstance::OnEquipped()
{
	if (AOvrlCharacterBase* OwningPawn = Cast<AOvrlCharacterBase>(GetOwner()))
	{
		bIsEquipped = true;

		// Attach Display Mesh to 3rd person mesh
		OwningPawn->EquipObject(this, DisplayMesh.Get());

		// Play equip montage
		const UOvrlEquipmentDefinition* EquipmentDefinition = GetDefault<UOvrlEquipmentDefinition>(EquipmentDefinitionClass);
		OwningPawn->PlayAnimMontage(EquipmentDefinition->EquipMontage);

		// Apply anim layer class of the equip instance, used for 1st person mesh
		ApplyOverlayAnimInstance();

		SetActorHiddenInGame(false);
		K2_OnEquipped();
	}
}

void AOvrlEquipmentInstance::OnUnequipped()
{
	bIsEquipped = false;

	SetActorHiddenInGame(true);
	K2_OnUnequipped();
}

void AOvrlEquipmentInstance::ApplyOverlayAnimInstance()
{
	if (AOvrlCharacterBase* OwningPawn = Cast<AOvrlCharacterBase>(GetOwner()))
	{
		const UOvrlEquipmentDefinition* EquipmentDefinition = GetDefault<UOvrlEquipmentDefinition>(EquipmentDefinitionClass);
		OwningPawn->ApplyAnimLayerClass(EquipmentDefinition->OverlayAnimInstance);
	}
}


// Fill out your copyright notice in the Description page of Project Settings.

#include "Equipment/OvrlEquipmentInstance.h"

#include "Equipment/OvrlEquipmentDefinition.h"
#include "Animations/OvrlLinkedAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Inventory/OvrlItemDefinition.h"
#include "Inventory/OvrlItemInstance.h"

#include "Player/OvrlCharacterBase.h"

#include "OvrlLogUtils.h"

// Sets default values
AOvrlEquipmentInstance::AOvrlEquipmentInstance()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

void AOvrlEquipmentInstance::BeginPlay()
{
	Super::BeginPlay();

	SetActorTickEnabled(false); // Will be enabled when equipped
}

void AOvrlEquipmentInstance::Initialize(const TSubclassOf<UOvrlEquipmentDefinition>& InEquipmentDefinitionClass, UOvrlItemInstance* InAssociatedItem)
{
	EquipmentDefinitionClass = InEquipmentDefinitionClass;
	AssociatedItem = InAssociatedItem;

	if (ensure(AssociatedItem) && AssociatedItem->GetItemDef())
	{
		DisplayMesh = AssociatedItem->GetItemDef()->DisplayMesh;
	}
}

void AOvrlEquipmentInstance::OnEquipped_Implementation()
{
	bIsEquipped = true;

	SetActorHiddenInGame(false);
	SetActorTickEnabled(true);

	if (AOvrlCharacterBase* OwningPawn = Cast<AOvrlCharacterBase>(GetOwner()))
	{
		OwnerSkeletalMesh = OwningPawn->GetMesh();
		
		const UOvrlEquipmentDefinition* EquipmentDefinition = GetDefault<UOvrlEquipmentDefinition>(EquipmentDefinitionClass);

		// Attach Display Mesh to 3rd person mesh
		OwningPawn->EquipObject(this, EquipmentDefinition->AttachSocketName, DisplayMesh.Get());
	}
}

void AOvrlEquipmentInstance::OnBeforeUnequip_Implementation()
{
}

void AOvrlEquipmentInstance::OnUnequipped_Implementation()
{
	bIsEquipped = false;

	// SetActorTickEnabled(false);
	SetActorHiddenInGame(true);
}

float AOvrlEquipmentInstance::GetEquipNotifyTime() const
{
	const UOvrlEquipmentDefinition* EquipmentDefinition = GetDefault<UOvrlEquipmentDefinition>(EquipmentDefinitionClass);

	if (!EquipmentDefinition->EquipMontage)
	{
		// No Montage -> instant item switch
		OVRL_LOG_WARN(LogOverlink, true, "Unable to find equip montage for item %s.", *GetName());
		return 0.f;
	}

	// Search if we find an equip notify, returning the time at where the notify is placed in the montage
	for (const FAnimNotifyEvent& NotifyEvent : EquipmentDefinition->EquipMontage->Notifies)
	{
		if (NotifyEvent.NotifyName == EquipmentDefinition->EquipNotifyName)
		{
			return NotifyEvent.GetTriggerTime();
		}
	}

	// If can't find the notify, return a default value
	constexpr float DefaultEquipNotifyTime = .5f;
	OVRL_LOG_WARN(LogOverlink, true, "Unable to find notify named '%s' in the montage '%s'. As default behavior, %f seconds will be used.",
		*EquipmentDefinition->EquipNotifyName.ToString(),
		*EquipmentDefinition->EquipMontage->GetName(),
		DefaultEquipNotifyTime);

	return DefaultEquipNotifyTime;
}

void AOvrlEquipmentInstance::PlayEquipMontage() const
{
	if (AOvrlCharacterBase* OwningPawn = Cast<AOvrlCharacterBase>(GetOwner()))
	{
		// Play equip montage
		const UOvrlEquipmentDefinition* EquipmentDefinition = GetDefault<UOvrlEquipmentDefinition>(EquipmentDefinitionClass);
		OwningPawn->OvrlPlayAnimMontage(EquipmentDefinition->EquipMontage);

		// Apply anim layer class of the equip instance, used for 1st person mesh
		ApplyOverlayAnimInstance();
	}
}

void AOvrlEquipmentInstance::ApplyOverlayAnimInstance() const
{
	if (AOvrlCharacterBase* OwningPawn = Cast<AOvrlCharacterBase>(GetOwner()))
	{
		const UOvrlEquipmentDefinition* EquipmentDefinition = GetDefault<UOvrlEquipmentDefinition>(EquipmentDefinitionClass);
		OwningPawn->ApplyAnimLayerClass(EquipmentDefinition->OverlayAnimInstance);
	}
}

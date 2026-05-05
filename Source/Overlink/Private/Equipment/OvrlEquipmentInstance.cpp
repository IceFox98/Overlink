// Fill out your copyright notice in the Description page of Project Settings.

#include "Equipment/OvrlEquipmentInstance.h"

#include "AbilitySystem/OvrlAbilitySystemComponent.h"
#include "Equipment/OvrlEquipmentDefinition.h"
// #include "Animations/OvrlLinkedAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Inventory/OvrlItemDefinition.h"
#include "Inventory/OvrlItemInstance.h"
#include "Player/OvrlCharacterBase.h"
#include "OvrlLogUtils.h"

// Engine
#include "AbilitySystemGlobals.h"

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

void AOvrlEquipmentInstance::Initialize(UOvrlEquipmentDefinition* InEquipmentDefinition, UOvrlItemInstance* InAssociatedItem)
{
	EquipmentDefinition = InEquipmentDefinition;
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

	if (!EquipmentDefinition)
	{
		return;
	}

	if (AOvrlCharacterBase* OwningPawn = Cast<AOvrlCharacterBase>(GetOwner()))
	{
		OwnerSkeletalMesh = OwningPawn->GetMesh();

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

	SetActorHiddenInGame(true);

	// if (UOvrlAbilitySystemComponent* ASC = GetOwnerAbilitySystemComponent())
	// {
	// 	// When unequip the item, remove all given abilities/effects/attributes from owner's ASC
	// 	GrantedHandles.TakeFromAbilitySystem(ASC);
	// }
}

void AOvrlEquipmentInstance::BeginDrop_Implementation()
{
	// Play any sound, or other logic

	OnDropReady.Broadcast(this);
}

float AOvrlEquipmentInstance::GetEquipNotifyTime() const
{
	if (!EquipmentDefinition)
	{
		return 0.f;
	}

	if (!EquipmentDefinition->bPlayMontageOnEquip)
	{
		// Should not play any montage -> instant item switch
		OVRL_LOG_WARN(LogOverlink, true, "bPlayMontageOnEquip is disable, equip time will be 0s");
		return 0.f;
	}

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
		if (EquipmentDefinition && EquipmentDefinition->bPlayMontageOnEquip)
		{
			OwningPawn->OvrlPlayAnimMontage(EquipmentDefinition->EquipMontage);
		}

		// Apply anim layer class of the equip instance, used for 1st person mesh
		ApplyOverlayAnimInstance();
	}
}

void AOvrlEquipmentInstance::ApplyOverlayAnimInstance() const
{
	if (AOvrlCharacterBase* OwningPawn = Cast<AOvrlCharacterBase>(GetOwner()))
	{
		if (EquipmentDefinition)
		{
			OwningPawn->ApplyAnimLayerClass(EquipmentDefinition->OverlayAnimInstance);
		}
	}
}

UOvrlAbilitySystemComponent* AOvrlEquipmentInstance::GetOwnerAbilitySystemComponent() const
{
	return Cast<UOvrlAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()));
}

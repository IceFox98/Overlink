// Fill out your copyright notice in the Description page of Project Settings.

#include "Equipment/OvrlEquipmentInstance.h"

#include "OvrlUtils.h"
#include "Equipment/OvrlEquipmentDefinition.h"
#include "Animations/OvrlLinkedAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

#include "Player/OvrlCharacterBase.h"

#include "OvrlUtils.h"
#include "Overlink.h"

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
		OwningPawn->PlayAnimMontage(EquipmentDefinition->EquipMontage);
		
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

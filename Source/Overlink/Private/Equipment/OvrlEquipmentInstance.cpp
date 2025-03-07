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

void AOvrlEquipmentInstance::OnEquipped()
{
	if (AOvrlCharacterBase* OwningPawn = Cast<AOvrlCharacterBase>(GetOwner()))
	{
		AttachToComponent(OwningPawn->GetEquipAttachmentComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, OwningPawn->GripPointName);
		// TODO: Spanw and attach skeletal mesh to FullBody mesh

		bIsEquipped = true;

		ApplyOverlayAnimation();

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

void AOvrlEquipmentInstance::ApplyOverlayAnimation()
{
	if (AOvrlCharacterBase* OwningPawn = Cast<AOvrlCharacterBase>(GetOwner()))
	{
		const UOvrlEquipmentDefinition* EquipmentDefinition = GetDefault<UOvrlEquipmentDefinition>(EquipmentDefinitionClass);
		OwningPawn->ApplyAnimClassLayer(EquipmentDefinition->OverlayAnimInstance);
	}
}


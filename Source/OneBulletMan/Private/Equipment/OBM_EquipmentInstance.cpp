// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/OBM_EquipmentInstance.h"
#include "Equipment/OBM_EquipmentDefinition.h"

#include "Player/OBM_CharacterBase.h"

// Sets default values
AOBM_EquipmentInstance::AOBM_EquipmentInstance()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AOBM_EquipmentInstance::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AOBM_EquipmentInstance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AOBM_EquipmentInstance::OnEquipped()
{
	if (AOBM_CharacterBase* OwningPawn = Cast<AOBM_CharacterBase>(GetOwner()))
	{
		AttachToComponent(OwningPawn->GetItemHoldingComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, OwningPawn->GripPointName);

		const FTransform RelativeTransform = GetDefault<UOBM_EquipmentDefinition>(EquipmentDefinition)->RelativeTransform;
		SetActorRelativeRotation(RelativeTransform.GetRotation());
		SetActorRelativeLocation(RelativeTransform.GetLocation());

		bIsEquipped = true;

		SetActorHiddenInGame(false);
		K2_OnEquipped();
	}
}

void AOBM_EquipmentInstance::OnUnequipped()
{
	bIsEquipped = false;

	SetActorHiddenInGame(true);
	K2_OnUnequipped();
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/EquipmentInstance.h"

#include "Player/CharacterBase.h"

// Sets default values
AEquipmentInstance::AEquipmentInstance()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEquipmentInstance::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AEquipmentInstance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEquipmentInstance::OnEquipped()
{
	if (ACharacterBase* OwningPawn = Cast<ACharacterBase>(GetOwner()))
	{
		AttachToComponent(OwningPawn->GetItemHoldingComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, OwningPawn->GripPointName);

		//const FTransform RelativeTransform = GetDefault<UEquipmentDefinition>(EquipmentDefinition)->RelativeTransform.GetRotation();

		bIsEquipped = true;

		SetActorHiddenInGame(false);
		K2_OnEquipped();
	}
}

void AEquipmentInstance::OnUnequipped()
{
	bIsEquipped = false;

	SetActorHiddenInGame(true);
	K2_OnUnequipped();
}


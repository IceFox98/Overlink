// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/EquipmentInstance.h"
#include "Equipment/EquipmentDefinition.h"
#include "Camera/CameraComponent.h"

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
	if (APawn* OwningPawn = Cast<APawn>(GetOwner()))
	{
		USceneComponent* AttachTarget = OwningPawn->GetComponentByClass<UCameraComponent>();
		if (!AttachTarget)
		{
			AttachTarget = OwningPawn->GetRootComponent();
		}

		AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform);
		
		FTransform RelativeTransform = GetDefault<UEquipmentDefinition>(EquipmentDefinition)->RelativeTransform;
		SetActorRelativeTransform(RelativeTransform);
	}

	SetActorHiddenInGame(false);

	K2_OnEquipped();
}

void AEquipmentInstance::OnUnequipped()
{
	SetActorHiddenInGame(true);

	K2_OnUnequipped();
}


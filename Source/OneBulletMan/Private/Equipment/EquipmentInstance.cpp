// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/EquipmentInstance.h"
#include "Equipment/EquipmentDefinition.h"
#include "Camera/CameraComponent.h"

#include "Kismet/KismetMathLibrary.h"

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

	if (IsEquipped())
	{
		// Follow target
		const FTransform TargetTransform = TargetToFollow->GetComponentTransform();

		const FTransform RelativeTransform = GetDefault<UEquipmentDefinition>(EquipmentDefinition)->RelativeTransform;
		const FTransform FinalTransform = UKismetMathLibrary::TInterpTo(GetActorTransform(), RelativeTransform * TargetTransform, DeltaTime, 10.f);

		SetActorTransform(FinalTransform);
	}
}

void AEquipmentInstance::OnEquipped()
{
	if (!TargetToFollow) // First time equipping?
	{
		if (APawn* OwningPawn = Cast<APawn>(GetOwner()))
		{
			TargetToFollow = OwningPawn->GetComponentByClass<UCameraComponent>();
			if (!TargetToFollow)
			{
				TargetToFollow = OwningPawn->GetRootComponent();
			}

			check(TargetToFollow); // If even here is null, there's could be a problem...


		}
	}

	const FTransform RelativeTransform = GetDefault<UEquipmentDefinition>(EquipmentDefinition)->RelativeTransform;
	const FTransform TargetTransform = TargetToFollow->GetComponentTransform();

	// Convert the RelativeTransform to world space
	SetActorTransform(RelativeTransform * TargetTransform);

	bIsEquipped = true;

	SetActorHiddenInGame(false);
	K2_OnEquipped();
}

void AEquipmentInstance::OnUnequipped()
{
	bIsEquipped = false;

	SetActorHiddenInGame(true);
	K2_OnUnequipped();
}


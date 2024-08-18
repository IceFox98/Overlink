// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/EquipmentInstance.h"
#include "Equipment/EquipmentDefinition.h"
#include "Camera/CameraComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

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

	//if (IsEquipped())
	//{
	//	if (ACharacterBase* OwningPawn = Cast<ACharacterBase>(GetOwner()))
	//	{
	//		// Follow target
	//		FTransform TargetTransform = TargetToFollow->GetComponentTransform();

	//		const FQuat RelativeRotation = GetDefault<UEquipmentDefinition>(EquipmentDefinition)->RelativeTransform.GetRotation();
	//		FQuat TargetRotation = TargetTransform.TransformRotation(RelativeRotation);
	//		//FQuat SmoothedRotation = FMath::QInterpTo(GetActorQuat(), TargetRotation, DeltaTime, 10.f);
	//		SetActorRotation(TargetRotation);

	//		FVector TargetLocation = TargetTransform.TransformPosition(RelativeLocation);

	//		UE_LOG(LogTemp, Warning, TEXT("%s"), *TargetLocation.ToString());

	//		DrawDebugPoint(GetWorld(), TargetLocation, 10.f, FColor::Red, true);

	//		const FVector FinalLocation = UKismetMathLibrary::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime, 20.f);

	//		SetActorLocation(FinalLocation);

	//	}
	//}
}

void AEquipmentInstance::OnEquipped()
{
	//if (!TargetToFollow) // First time equipping?
	//{
	if (ACharacterBase* OwningPawn = Cast<ACharacterBase>(GetOwner()))
	{
		//TargetToFollow = OwningPawn->GetComponentByClass<UCameraComponent>();
		//if (!TargetToFollow)
		//{
		//	TargetToFollow = OwningPawn->GetRootComponent();
		//}

		//check(TargetToFollow); // If even here is null, there could be a problem...


		//const FTransform RelativeTransform = GetDefault<UEquipmentDefinition>(EquipmentDefinition)->RelativeTransform;
		//const FTransform TargetTransform = TargetToFollow->GetComponentTransform();

		//// Convert the RelativeTransform to world space
		//SetActorTransform(RelativeTransform * TargetTransform);

		//RelativeLocation = GetDefault<UEquipmentDefinition>(EquipmentDefinition)->RelativeTransform.GetLocation();

		AttachToComponent(OwningPawn->GetItemHoldingComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("GripPoint"));

		bIsEquipped = true;

		SetActorHiddenInGame(false);
		K2_OnEquipped();
	}
	//}
}

void AEquipmentInstance::OnUnequipped()
{
	bIsEquipped = false;

	SetActorHiddenInGame(true);
	K2_OnUnequipped();
}


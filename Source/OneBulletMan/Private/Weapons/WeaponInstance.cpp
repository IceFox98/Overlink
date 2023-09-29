// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WeaponInstance.h"
#include "Components/SphereComponent.h"

AWeaponInstance::AWeaponInstance()
{
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(WeaponMesh);

}

void AWeaponInstance::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);


}

void AWeaponInstance::UpdateFiringTime()
{
	TimeLastFired = GetWorld()->GetTimeSeconds();
}

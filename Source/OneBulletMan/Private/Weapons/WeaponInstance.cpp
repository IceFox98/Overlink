// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WeaponInstance.h"
#include "Components/SphereComponent.h"

AWeaponInstance::AWeaponInstance()
{
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupSphere"));
	PickupSphere->SetupAttachment(RootComponent);
}

void AWeaponInstance::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);


}

void AWeaponInstance::Fire()
{

}

void AWeaponInstance::Reload()
{

}

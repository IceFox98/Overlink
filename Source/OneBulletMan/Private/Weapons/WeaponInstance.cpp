// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WeaponInstance.h"

AWeaponInstance::AWeaponInstance()
{
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
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

// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/OvrlRangedWeaponInstance.h"
#include "Kismet/KismetMathLibrary.h"

AOvrlRangedWeaponInstance::AOvrlRangedWeaponInstance()
{
	BulletsPerCartridge = 1;
	MaxDamageRange = 25000.0f;
	FireRate = 400.f;

	KickbackRecoverySpeed = 10.f;
	CameraRecoilRecoverySpeed = 10.f;
	CameraMaxRecoil = 20.f;
}

void AOvrlRangedWeaponInstance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentKickbackRecoil = UKismetMathLibrary::TInterpTo(CurrentKickbackRecoil, FTransform::Identity, DeltaTime, KickbackRecoverySpeed);

	if (CurrentCameraRecoil > 0.f)
	{
		CurrentCameraRecoil -= CameraRecoilRecoverySpeed * DeltaTime;
		CurrentCameraRecoil = FMath::Max(CurrentCameraRecoil, 0.f);

		//GetOwner()->GetInstigatorController()->AddController
	}
}

void AOvrlRangedWeaponInstance::Fire(const FHitResult& HitData)
{
	Super::Fire(HitData);

	CurrentKickbackRecoil = KickbackRecoil;

	CurrentCameraRecoil = FMath::Clamp(CurrentCameraRecoil + CameraRecoil, 0.f, CameraMaxRecoil);
}

void AOvrlRangedWeaponInstance::AddSpread()
{

}

FTransform AOvrlRangedWeaponInstance::GetMuzzleTransform() const
{
	check(WeaponMesh);
	return WeaponMesh->GetSocketTransform(MuzzleSocketName);
}

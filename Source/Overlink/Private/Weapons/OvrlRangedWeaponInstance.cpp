// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/OvrlRangedWeaponInstance.h"

#include "Kismet/KismetMathLibrary.h"
#include "Curves/CurveFloat.h"

AOvrlRangedWeaponInstance::AOvrlRangedWeaponInstance()
{
	BulletsPerCartridge = 1;
	MaxDamageRange = 25000.0f;
	FireRate = 400.f;

	KickbackRecoverySpeed = 10.f;
	CameraRecoilRecoverySpeed = 10.f;
	CameraMaxRecoil = 20.f;

	MinSpreadAngle = 24.f;
	MaxSpreadAngle = 50.f;
	SpreadPerShot = 1.f;
	SpreadRecoverySpeed = 1.f;
}

void AOvrlRangedWeaponInstance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateRecoil(DeltaTime);
	UpdateSpread(DeltaTime);
}

void AOvrlRangedWeaponInstance::OnEquipped()
{
	Super::OnEquipped();

	SpreadHeat = MinSpreadAngle;
}

void AOvrlRangedWeaponInstance::Fire(const FHitResult& HitData)
{
	Super::Fire(HitData);

	CurrentKickbackRecoil = KickbackRecoil;
	CurrentCameraRecoil = FMath::Clamp(CurrentCameraRecoil + CameraRecoil, 0.f, CameraMaxRecoil);
	LastFireTime = GetWorld()->GetTimeSeconds();

	AddSpread();
}

void AOvrlRangedWeaponInstance::AddSpread()
{
	const float NormalizedHeat = UKismetMathLibrary::NormalizeToRange(SpreadHeat, MinSpreadAngle, MaxSpreadAngle);
	SpreadHeat += SpreadPerShot * HeatToHeatMultiplier.GetRichCurve()->Eval(NormalizedHeat);
}

void AOvrlRangedWeaponInstance::UpdateRecoil(float DeltaTime)
{
	CurrentKickbackRecoil = UKismetMathLibrary::TInterpTo(CurrentKickbackRecoil, FTransform::Identity, DeltaTime, KickbackRecoverySpeed);

	if (CurrentCameraRecoil > 0.f)
	{
		CurrentCameraRecoil -= CameraRecoilRecoverySpeed * DeltaTime;
		CurrentCameraRecoil = FMath::Max(CurrentCameraRecoil, 0.f);

		//GetOwner()->GetInstigatorController()->AddController
	}
}

void AOvrlRangedWeaponInstance::UpdateSpread(float DeltaTime)
{
	const double TargetSpread = FMath::Min(MinSpreadAngle + SpreadHeat, MaxSpreadAngle);
	CurrentSpread = FMath::FInterpTo(CurrentSpread, TargetSpread, DeltaTime, 40.f);

	if (GetWorld()->TimeSince(LastFireTime) > .05)
	{
		SpreadHeat = FMath::FInterpTo(SpreadHeat, 0.f, DeltaTime, SpreadRecoverySpeed);
	}
}

FTransform AOvrlRangedWeaponInstance::GetMuzzleTransform() const
{
	check(WeaponMesh);
	return WeaponMesh->GetSocketTransform(MuzzleSocketName);
}
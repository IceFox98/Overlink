// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/OvrlRangedWeaponInstance.h"

#include "Kismet/KismetMathLibrary.h"
#include "Curves/CurveFloat.h"

#include "OvrlUtils.h"

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
	SpreadRecoveryCooldownDelay = .2f;
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

	CurrentSpread = MinSpreadAngle;
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
	// Normalize current heat to a 0-1 range
	CurrentHeat += HeatToHeatPerShot.GetRichCurve()->Eval(CurrentHeat);
	
	const float NormalizedHeat = UKismetMathLibrary::NormalizeToRange(CurrentHeat, 0.f, 100.f);
	CurrentSpread = HeatToSpread.GetRichCurve()->Eval(NormalizedHeat);
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
	// Fastly smooth the spread, so the weapon reticle spokes don't snap to the spread angle
	//const double TargetSpread = FMath::Min(MinSpreadAngle + SpreadHeat, MaxSpreadAngle);
	const float NormalizedHeat = UKismetMathLibrary::NormalizeToRange(CurrentHeat, 0.f, 100.f);
	CurrentSpread = HeatToSpread.GetRichCurve()->Eval(NormalizedHeat);

	//if (GetWorld()->TimeSince(LastFireTime) > SpreadRecoveryCooldownDelay)
	{
		CurrentHeat = FMath::Clamp(CurrentHeat - SpreadRecoverySpeed * DeltaTime, 0.f, 100.f);
		//CurrentSpread = FMath::Clamp(CurrentSpread - SpreadRecoverySpeed * DeltaTime, HeatToSpread.GetRichCurve()->Eval(0.f), HeatToSpread.GetRichCurve()->Eval(1.f));
		//OVRL_LOG("%f", CurrentHeat);
	}
}

FTransform AOvrlRangedWeaponInstance::GetMuzzleTransform() const
{
	check(WeaponMesh);
	return WeaponMesh->GetSocketTransform(MuzzleSocketName);
}

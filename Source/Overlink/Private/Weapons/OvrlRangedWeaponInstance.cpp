// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/OvrlRangedWeaponInstance.h"

#include "Player/Components/OvrlCharacterMovementComponent.h"

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

	SpreadRecoverySpeed = 1.f;

	// Multipliers
	SpreadMultiplier = 1.f;
	SpreadMultiplierWalking = 1.2f;
	SpreadMultiplierRunning = 1.4f;
	SpreadMultiplierCrouchStanding = .5f;
	SpreadMultiplierCrouchWalking = .75f;
	SpreadMultiplierFalling = 2.f;
}

void AOvrlRangedWeaponInstance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateSpreadMultiplier(DeltaTime);
	UpdateRecoil(DeltaTime);
	UpdateSpread(DeltaTime);
}

#if WITH_EDITOR
void AOvrlRangedWeaponInstance::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	MinSpreadAngle = HeatToSpread.GetRichCurve()->Eval(0.f);
	MaxSpreadAngle = HeatToSpread.GetRichCurve()->Eval(1.f);
}
#endif

void AOvrlRangedWeaponInstance::OnEquipped()
{
	Super::OnEquipped();

	CurrentSpread = HeatToSpread.GetRichCurve()->Eval(0.f);

	OwnerMovementComp = GetOwner()->GetComponentByClass<UOvrlCharacterMovementComponent>();
}

void AOvrlRangedWeaponInstance::Fire(const FHitResult& HitData)
{
	Super::Fire(HitData);

	CurrentKickbackRecoil = KickbackRecoil;
	CurrentCameraRecoil = FMath::Clamp(CurrentCameraRecoil + CameraRecoil, 0.f, CameraMaxRecoil);

	AddSpread();

	SpawnFireVFX(HitData);
	SpawnImpactVFX(HitData);
}

void AOvrlRangedWeaponInstance::AddSpread()
{
	CurrentHeat += HeatToHeatPerShot.GetRichCurve()->Eval(CurrentHeat) ;

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
	const float NormalizedHeat = UKismetMathLibrary::NormalizeToRange(CurrentHeat, 0.f, 100.f);
	CurrentSpread = HeatToSpread.GetRichCurve()->Eval(NormalizedHeat) * SpreadMultiplier;

	CurrentHeat = FMath::Clamp(CurrentHeat - SpreadRecoverySpeed * DeltaTime, 0.f, 100.f);
	//OVRL_LOG("%f", CurrentHeat);
}

void AOvrlRangedWeaponInstance::UpdateSpreadMultiplier(float DeltaTime)
{
	float TargetMultiplier = 1.f;

	if (OwnerMovementComp)
	{
		const float Velocity = OwnerMovementComp->GetLastUpdateVelocity().Length();
		const bool bIsWalking = Velocity > 0.f && Velocity < 200.f;
		const bool bIsRunning = Velocity > 200.f;

		const bool bIsMoving = bIsWalking || bIsRunning;

		if (OwnerMovementComp->IsFalling())
		{
			TargetMultiplier = SpreadMultiplierFalling;
		}
		else if (OwnerMovementComp->IsCrouching())
		{
			TargetMultiplier = bIsMoving ? SpreadMultiplierCrouchWalking : SpreadMultiplierCrouchStanding;
		}
		else // Standing
		{
			if (bIsMoving)
			{
				TargetMultiplier = bIsWalking ? SpreadMultiplierWalking : SpreadMultiplierRunning;
			}
		}
	}

	SpreadMultiplier = FMath::FInterpTo(SpreadMultiplier, TargetMultiplier, DeltaTime, 10.f);
}

void AOvrlRangedWeaponInstance::SpawnFireVFX(const FHitResult& HitData)
{
	ensureMsgf(MuzzleFlashVFX, TEXT("A ranged weapon should have a MuzzleFlashVFX set!"));
	const FTransform MuzzleTransform = GetMuzzleTransform();
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, MuzzleFlashVFX, MuzzleTransform.GetLocation(), MuzzleTransform.GetRotation().Rotator(), FVector::OneVector);
}

FTransform AOvrlRangedWeaponInstance::GetMuzzleTransform() const
{
	check(WeaponMesh);
	return WeaponMesh->GetSocketTransform(MuzzleSocketName); // World Space
}

FVector AOvrlRangedWeaponInstance::GetAimPosition() const
{
	check(WeaponMesh);
	return WeaponMesh->GetSocketTransform("AimSocket", ERelativeTransformSpace::RTS_Component).GetLocation();
}

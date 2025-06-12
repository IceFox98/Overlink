// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/OvrlRangedWeaponInstance.h"

#include "Player/Components/OvrlCharacterMovementComponent.h"
#include "Core/OvrlPlayerController.h"

#include "Kismet/KismetMathLibrary.h"
#include "Curves/CurveFloat.h"

#include "OvrlUtils.h"

AOvrlRangedWeaponInstance::AOvrlRangedWeaponInstance()
{
	BulletsPerCartridge = 1;
	MaxDamageRange = 25000.0f;
	FireRate = 400.f;
	AimSpeed = 25.f;

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


	//if (!bIsRecoiling)
	//{
	//	bIsRecoiling = true;
	//	RecoilStartRotation = PlayerController->GetControlRotation();
	//	AccumulatedRecoil = FRotator::ZeroRotator;
	//}

	//AccumulatedRecoil += FRotator(CameraRecoil, 0.f, 0.f);

	//// Applica il rinculo immediato
	//FRotator NewRot = PlayerController->GetControlRotation() + FRotator(CameraRecoil, 0.f, 0.f);
	//PlayerController->SetControlRotation(NewRot);

	AController* PlayerController = GetOwner()->GetInstigatorController();

	if (FMath::IsNearlyZero(CurrentCameraRecoil))
	{
		RecoilStartRotation = PlayerController->GetControlRotation();
	}

	CurrentCameraRecoil = FMath::Clamp(CurrentCameraRecoil + CameraRecoil, 0.f, CameraMaxRecoil);
	PlayerController->SetControlRotation(PlayerController->GetControlRotation() + FRotator(CurrentCameraRecoil, 0.f, 0.f));

	CurrentKickbackRecoil = KickbackRecoil;

	AddSpread();

	SpawnFireVFX(HitData);
	SpawnImpactVFX(HitData);
}

void AOvrlRangedWeaponInstance::AddSpread()
{
	CurrentHeat += HeatToHeatPerShot.GetRichCurve()->Eval(CurrentHeat);

	const float NormalizedHeat = UKismetMathLibrary::NormalizeToRange(CurrentHeat, 0.f, 100.f);
	CurrentSpread = HeatToSpread.GetRichCurve()->Eval(NormalizedHeat);
}

void AOvrlRangedWeaponInstance::UpdateRecoil(float DeltaTime)
{
	CurrentKickbackRecoil = UKismetMathLibrary::TInterpTo(CurrentKickbackRecoil, FTransform::Identity, DeltaTime, KickbackRecoverySpeed);

	if (CurrentCameraRecoil > 0.f)
	{
		AOvrlPlayerController* PlayerController = Cast<AOvrlPlayerController>(GetOwner()->GetInstigatorController());
		RecoilStartRotation += PlayerController->GetLastRotationInput();

		//	FRotator Current = GetOwner()->GetInstigatorController()->GetControlRotation();

		//	//// Applica parte del rinculo gradualmente
		//	//FRotator RecoilThisFrame = RecoilSpeed * DeltaTime;

		//	//ControlRotation.Pitch += CurrentCameraRecoil;
		//	//ControlRotation.Yaw -= RecoilThisFrame.Yaw;

		//Cast<APawn>(GetOwner())->AddControllerPitchInput(-CurrentCameraRecoil * DeltaTime);

		FRotator CurrentRot = PlayerController->GetControlRotation();
		FRotator TargetRot = FMath::RInterpTo(CurrentRot, RecoilStartRotation, DeltaTime, CameraRecoilRecoverySpeed);
		PlayerController->SetControlRotation(TargetRot);

		CurrentCameraRecoil = UKismetMathLibrary::FInterpTo(CurrentCameraRecoil, 0.f, DeltaTime, CameraRecoilRecoverySpeed);

		//	//CurrentCameraRecoil -= CameraRecoilRecoverySpeed * DeltaTime;
		//	//CurrentCameraRecoil = FMath::Max(CurrentCameraRecoil, 0.f);
	}


	//OVRL_LOG("%s", *PlayerController->GetLastRotationInput().ToString());

	//if (!PlayerController || AccumulatedRecoil.IsNearlyZero()) return;

	//FRotator PlayerDelta = CurrentRot - (RecoilStartRotation + AccumulatedRecoil);
	//FRotator RemainingRecoil = AccumulatedRecoil - PlayerDelta;

	//// Ritorna solo la parte non compensata dal giocatore

	//FRotator TargetRot = RecoilStartRotation + AccumulatedRecoil;
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

	if (IsADS())
	{
		TargetMultiplier = 0.f;
	}
	else
	{
		if (OwnerMovementComp)
		{
			const float Velocity = OwnerMovementComp->GetLastUpdateVelocity().Length();
			const bool bIsRunning = OwnerMovementComp->GetGait() == OvrlGaitTags::Running;
			const bool bIsWalking = Velocity > 0.f && !bIsRunning;

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

FTransform AOvrlRangedWeaponInstance::GetAimTransform() const
{
	check(WeaponMesh);
	return WeaponMesh->GetSocketTransform("AimSocket", ERelativeTransformSpace::RTS_Component);
}

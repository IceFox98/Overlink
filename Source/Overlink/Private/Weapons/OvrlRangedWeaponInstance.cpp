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

	AddRecoil();

	if (!IsADS())
	{
		AddSpread();
	}

	SpawnFireVFX(HitData);
	SpawnImpactVFX(HitData);
}

void AOvrlRangedWeaponInstance::StopFire()
{
	Super::StopFire();

	bCanRecoverFromRecoil = true;
}

void AOvrlRangedWeaponInstance::AddSpread()
{
	CurrentHeat += HeatToHeatPerShot.GetRichCurve()->Eval(CurrentHeat);

	const float NormalizedHeat = UKismetMathLibrary::NormalizeToRange(CurrentHeat, 0.f, 100.f);
	CurrentSpread = HeatToSpread.GetRichCurve()->Eval(NormalizedHeat);
}

void AOvrlRangedWeaponInstance::AddRecoil()
{
	CurrentCameraRecoil.Pitch = FMath::Clamp(CurrentCameraRecoil.Pitch + CameraRecoil, 0.f, CameraMaxRecoil);
	CurrentKickbackRecoil = KickbackRecoil;
	bCanRecoverFromRecoil = false;
}

void AOvrlRangedWeaponInstance::UpdateRecoil(float DeltaTime)
{
	CurrentKickbackRecoil = UKismetMathLibrary::TInterpTo(CurrentKickbackRecoil, FTransform::Identity, DeltaTime, KickbackRecoverySpeed);

	if (CurrentCameraRecoil.Pitch > 0.f || CurrentCameraRecoil.Yaw > 0.f)
	{
		AOvrlPlayerController* PlayerController = Cast<AOvrlPlayerController>(GetOwner()->GetInstigatorController());
		if (!PlayerController)
			return;

		// Get the controller Delta Rotation to know how much the recoil has been applied, considering the eventual player mouse compensation
		const FRotator ControllerDeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(PlayerController->GetControlRotation(), LastControllerRotation);
		const FRotator MouseDelta = ControllerDeltaRotation - RecoilStep;
		FRotator FinalDelta = ControllerDeltaRotation;

		// Remove positive mouse compensation from delta, so that it will not be considered as "additional" recoil to recover
		if (MouseDelta.Pitch > 0.f || MouseDelta.Yaw > 0.f)
		{
			FinalDelta -= MouseDelta;
		}

		DeltaRotation += FinalDelta;

		// Consider only the positive delta, because maybe the player has over-compensated the recoil
		DeltaRotation.Pitch = FMath::Clamp(DeltaRotation.Pitch, 0.f, 360.f);
		DeltaRotation.Yaw = FMath::Clamp(DeltaRotation.Yaw, 0.f, 360.f);

		// Going from zero to target since we're adding recoil each frame
		RecoilStep = UKismetMathLibrary::RInterpTo_Constant(FRotator::ZeroRotator, CurrentCameraRecoil, DeltaTime, CameraRecoilRecoverySpeed);
		OwnerMovementComp->GetPawnOwner()->AddControllerPitchInput(RecoilStep.Pitch);
		//OwnerMovementComp->GetPawnOwner()->AddControllerYawInput(RecoilStep.Yaw);
		CurrentCameraRecoil -= RecoilStep;

		LastControllerRotation = PlayerController->GetControlRotation();
	}
	else if (bCanRecoverFromRecoil)
	{
		AOvrlPlayerController* PlayerController = Cast<AOvrlPlayerController>(GetOwner()->GetInstigatorController());
		if (!PlayerController)
			return;

		// This logic handles the recoil recovery of the weapon
		const FRotator RecoilRecoveryStep = UKismetMathLibrary::RInterpTo_Constant(FRotator::ZeroRotator, DeltaRotation, DeltaTime, 30.f);
		OwnerMovementComp->GetPawnOwner()->AddControllerPitchInput(-RecoilRecoveryStep.Pitch);
		//OwnerMovementComp->GetPawnOwner()->AddControllerYawInput(-RecoilRecoveryStep.Yaw);
		DeltaRotation -= RecoilRecoveryStep;

		// Update last controller rotation to avoid bugs when new recoil is applied meanwhile the recovery is ongoing
		LastControllerRotation = PlayerController->GetControlRotation();
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

// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/OvrlRangedWeaponInstance.h"

#include "Core/OvrlPlayerCameraManager.h"
#include "Weapons/OvrlWeaponSightDefinition.h"
#include "Player/Components/OvrlCharacterMovementComponent.h"
#include "Player/Components/OvrlCameraComponent.h"
#include "Player/CameraModifiers/OvrlCameraModifier_FOV.h"
#include "Player/OvrlCharacterBase.h"
#include "Inventory/OvrlItemInstance.h"
#include "OvrlGameplayTags.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Curves/CurveFloat.h"
#include "Animation/AnimMontage.h"

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

	MuzzleSocketName = TEXT("Muzzle");
	AimSocketName = TEXT("Aim");
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

	PlayFireAnimation();
	SpawnImpactVFX(HitData);
}

void AOvrlRangedWeaponInstance::StopFire()
{
	Super::StopFire();

	bCanRecoverFromRecoil = true;
}

void AOvrlRangedWeaponInstance::StartReloading()
{
	Super::StartReloading();

	if (UAnimMontage* ReloadAnimMontage = ReloadMontage.LoadSynchronous())
	{
		if (AOvrlCharacterBase* Character = Cast<AOvrlCharacterBase>(GetOwner()))
		{
			Character->PlayAnimMontage(ReloadAnimMontage);
		}
	}
	else
	{
		ensureMsgf(false, TEXT("ReloadAnimMontage is NULL! The reload animation will be skipped and weapon will instantly reload."));
		PerformReload();
	}
}

void AOvrlRangedWeaponInstance::PerformReload()
{
	Super::PerformReload();

	if (UOvrlItemInstance* Item = GetAssociatedItem())
	{
		const int32 CurrentMagazineAmmo = Item->GetTagStackCount(OvrlWeaponTags::MagazineAmmo);
		const int32 MagazineSize = Item->GetTagStackCount(OvrlWeaponTags::MagazineSize);
		const int32 ShotsFired = MagazineSize - CurrentMagazineAmmo;

		Item->RemoveStack(OvrlWeaponTags::SpareAmmo, ShotsFired); // Decrease total ammo 
		Item->AddStack(OvrlWeaponTags::MagazineAmmo, ShotsFired); // Reset magazine ammo
	}
}

void AOvrlRangedWeaponInstance::ToggleADS(bool bEnable)
{
	bIsADS = bEnable;

	if (!ensure(SightDefinition))
	{
		return;
	}

	if (AOvrlPlayerCameraManager* CameraManager = AOvrlPlayerCameraManager::Get(this))
	{
		if (bIsADS)
		{
			if (ensure(SightDefinition->CameraFOV))
			{
				CameraFOV = CameraManager->GetOrAddCameraModifier<UOvrlCameraModifier_FOV>(SightDefinition->CameraFOV);
				if (CameraFOV)
				{
					// Enable every time since we're re-using the same modifier
					CameraFOV->EnableModifier();

					const float TargetFOV = SightDefinition->GetMagnifiedFOV(CameraManager->DefaultFOV);
					CameraFOV->SetTargetFOV(TargetFOV);
					CameraFOV->SetInterpSpeed(AimSpeed);
				}
			}
		}
		else if (CameraFOV)
		{
			CameraFOV->DisableModifier();
		}
	}
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
		APlayerController* PlayerController = Cast<APlayerController>(GetOwner()->GetInstigatorController());
		if (!PlayerController)
			return;

		// Get the controller Delta Rotation to know how much the recoil has been applied, considering the eventual player mouse compensation
		const FRotator ControllerDeltaRotation = UKismetMathLibrary::NormalizedDeltaRotator(PlayerController->GetControlRotation(), LastControllerRotation);
		const FRotator MouseDelta = ControllerDeltaRotation - RecoilStep;
		FRotator FinalDelta = ControllerDeltaRotation;

		// Remove positive mouse compensation from delta, so that it will not be considered as "additional" recoil to recover
		if (MouseDelta.Pitch > 0.f /*|| MouseDelta.Yaw > 0.f*/)
		{
			FinalDelta -= MouseDelta;
		}

		DeltaRotation += FinalDelta;

		// Consider only the positive delta, because maybe the player has over-compensated the recoil
		DeltaRotation.Pitch = FMath::Clamp(DeltaRotation.Pitch, 0.f, 360.f);
		//DeltaRotation.Yaw = FMath::Clamp(DeltaRotation.Yaw, 0.f, 360.f);

		// Going from zero to target since we're adding recoil each frame
		RecoilStep = UKismetMathLibrary::RInterpTo_Constant(FRotator::ZeroRotator, CurrentCameraRecoil, DeltaTime, CameraRecoilRecoverySpeed);
		OwnerMovementComp->GetPawnOwner()->AddControllerPitchInput(RecoilStep.Pitch);
		//OwnerMovementComp->GetPawnOwner()->AddControllerYawInput(RecoilStep.Yaw);
		CurrentCameraRecoil -= RecoilStep;

		LastControllerRotation = PlayerController->GetControlRotation();
	}
	else if (bCanRecoverFromRecoil)
	{
		APlayerController* PlayerController = Cast<APlayerController>(GetOwner()->GetInstigatorController());
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

void AOvrlRangedWeaponInstance::PlayFireAnimation()
{
	if (ensure(WeaponMesh && FireAnimation))
	{
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}
}

FTransform AOvrlRangedWeaponInstance::GetMuzzleTransform() const
{
	if (ensure(WeaponMesh))
	{
		return WeaponMesh->GetSocketTransform(MuzzleSocketName); // World Space
	}

	return FTransform::Identity;
}

FTransform AOvrlRangedWeaponInstance::GetAimTransform() const
{
	if (ensure(WeaponMesh && OwningSkeletalMesh))
	{
		const FTransform SocketTransform = WeaponMesh->GetSocketTransform(AimSocketName);
		FVector OutPosition;
		FRotator OutRotation;
		OwningSkeletalMesh->TransformToBoneSpace(OwnerAttachBoneName, SocketTransform.GetLocation(), SocketTransform.GetRotation().Rotator(), OutPosition, OutRotation);

		return { OutRotation, OutPosition, FVector::OneVector };
	}

	return FTransform::Identity;
}

int32 AOvrlRangedWeaponInstance::GetMagazineSize() const
{
	if (AssociatedItem)
	{
		return AssociatedItem->GetTagStackCount(OvrlWeaponTags::MagazineSize);
	}

	return 0.0f;
}

int32 AOvrlRangedWeaponInstance::GetMagazineAmmo() const
{
	if (AssociatedItem)
	{
		return AssociatedItem->GetTagStackCount(OvrlWeaponTags::MagazineAmmo);
	}

	return 0.0f;
}

int32 AOvrlRangedWeaponInstance::GetSpareAmmo() const
{
	if (AssociatedItem)
	{
		return AssociatedItem->GetTagStackCount(OvrlWeaponTags::SpareAmmo);
	}

	return 0.0f;
}

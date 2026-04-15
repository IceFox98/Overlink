// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/OvrlRangedWeaponInstance.h"

#include "Overlink.h"
#include "Core/OvrlPlayerCameraManager.h"
#include "Player/Components/OvrlCharacterMovementComponent.h"
#include "Player/CameraModifiers/OvrlCameraModifierBase.h"
#include "Player/OvrlCharacterBase.h"
#include "Inventory/OvrlItemInstance.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "Curves/CurveFloat.h"
#include "Animation/AnimMontage.h"

#include "Overlink.h"
#include "OvrlUtils.h"
#include "OvrlGameplayTags.h"

AOvrlRangedWeaponInstance::AOvrlRangedWeaponInstance()
{
	MagazineAmmoCountDisplay = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MagazineAmmoCountDisplay"));
	MagazineAmmoCountDisplay->SetupAttachment(WeaponMesh);
	MagazineAmmoCountDisplay->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BulletsPerCartridge = 1;
	MaxDamageRange = 25000.0f;
	FireRate = 400.f;
	AimTime = .15f;

	KickbackRecoverySpeed = 10.f;
	CameraRecoilRecoverySpeed = 10.f;
	CameraMaxRecoil = 20.f;

	SpreadRecoverySpeed = 1.f;

	// Spread Offsets
	SpreadOffset = 0.f;
	SpreadOffsetWalking = .8f;
	SpreadOffsetRunning = 1.2f;
	SpreadOffsetCrouchIdle = -.5f;
	SpreadOffsetCrouchWalking = -.2f;
	SpreadOffsetFalling = 3.f;

	MuzzleSocketName = TEXT("Muzzle");
	AimSocketName = TEXT("Aim");
}

void AOvrlRangedWeaponInstance::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateSpreadOffset(DeltaTime);
	UpdateRecoil(DeltaTime);
	UpdateSpread(DeltaTime);

	if (!IsEquipped() && DeltaRotation.IsNearlyZero())
	{
		// Disable ticking if the weapon is unequipped and all the recoil has been consumed
		SetActorTickEnabled(false);
	}
}

#if WITH_EDITOR
void AOvrlRangedWeaponInstance::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Just for visual information
	MinSpreadAngle = HeatToSpread.GetRichCurve()->Eval(0.f);
	MaxSpreadAngle = HeatToSpread.GetRichCurve()->Eval(1.f);
}
#endif

void AOvrlRangedWeaponInstance::OnEquipped_Implementation()
{
	Super::OnEquipped_Implementation();

	OwnerMovementComp = GetOwner()->GetComponentByClass<UOvrlCharacterMovementComponent>();
	bCanFire = true;

	UpdateMagazineAmmoCountDisplay();
}

void AOvrlRangedWeaponInstance::OnBeforeUnequip_Implementation()
{
	Super::OnBeforeUnequip_Implementation();

	StopFire();
	bCanFire = false;
	
	if (WeaponMesh)
	{
		// Stop weapon ongoing animation
		WeaponMesh->Stop();
		
		// Stop player reload animation
		if (AOvrlCharacterBase* Character = Cast<AOvrlCharacterBase>(GetOwner()))
		{
			Character->OvrlStopAnimMontage(PlayerReloadMontage.LoadSynchronous());
		}
	}
}

void AOvrlRangedWeaponInstance::Fire(const FHitResult& HitData)
{
	Super::Fire(HitData);

	AddRecoil();

	if (!IsADS())
	{
		AddSpread();
	}

	PlayWeaponAnimation(FireAnimation);
	SpawnImpactVFX(HitData);

	UpdateMagazineAmmoCountDisplay();
}

void AOvrlRangedWeaponInstance::StopFire()
{
	Super::StopFire();

	bCanRecoverFromRecoil = true;
}

void AOvrlRangedWeaponInstance::StartReloading()
{
	Super::StartReloading();

	if (UAnimMontage* PlayerReloadAnimMontage = PlayerReloadMontage.LoadSynchronous())
	{
		if (AOvrlCharacterBase* Character = Cast<AOvrlCharacterBase>(GetOwner()))
		{
			Character->OvrlPlayAnimMontage(PlayerReloadAnimMontage);
			PlayWeaponAnimation(ReloadAnimation);
		}
	}
	else
	{
		OVRL_LOG_WARN(LogOverlink, true, "ReloadAnimMontage is NULL! The reload animation will be skipped and weapon will instantly reload.");
		EndReloading();
	}
}

void AOvrlRangedWeaponInstance::EndReloading()
{
	if (!IsReloading())
	{
		return;
	}

	UpdateMagazineAmmoCountDisplay();

	Super::EndReloading();
}

void AOvrlRangedWeaponInstance::ToggleADS(bool bEnable)
{
	bIsADS = bEnable;

	if (AOvrlPlayerCameraManager* CameraManager = AOvrlPlayerCameraManager::Get(this))
	{
		if (bIsADS)
		{
			CameraFOV = CameraManager->GetOrAddCameraModifier<UOvrlCameraModifierBase>(UOvrlCameraModifierBase::StaticClass());
			if (CameraFOV)
			{
				// Enable every time since we're re-using the same modifier
				CameraFOV->EnableModifier();

				const float MagnifiedFOV = GetMagnifiedFOV(CameraManager->DefaultFOV); // E.g.: Default is 90, Magnified is 80 (zoom in)
				CameraFOV->SetCustomFOVOffset(MagnifiedFOV - CameraManager->DefaultFOV); // Offset will be -10, since camera modifier works with offset
				CameraFOV->SetAlphaTime(AimTime);
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

		// OVRL_LOG("%s", *RecoilStep.ToString());

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

		//OVRL_LOG("%s", *RecoilRecoveryStep.ToString());

		// Update last controller rotation to avoid bugs when new recoil is applied meanwhile the recovery is ongoing
		LastControllerRotation = PlayerController->GetControlRotation();
	}
}

void AOvrlRangedWeaponInstance::UpdateSpread(float DeltaTime)
{
	// Fastly smooth the spread, so the weapon reticle spokes don't snap to the spread angle
	const float NormalizedHeat = UKismetMathLibrary::NormalizeToRange(CurrentHeat, 0.f, 100.f);
	CurrentSpread = HeatToSpread.GetRichCurve()->Eval(NormalizedHeat) + SpreadOffset;

	CurrentHeat = FMath::Clamp(CurrentHeat - SpreadRecoverySpeed * DeltaTime, 0.f, 100.f);
}

void AOvrlRangedWeaponInstance::UpdateSpreadOffset(float DeltaTime)
{
	float TargetOffset = 0.f;

	if (IsADS())
	{
		TargetOffset = -MinSpreadAngle;
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
				TargetOffset = SpreadOffsetFalling;
			}
			else if (OwnerMovementComp->IsCrouching())
			{
				TargetOffset = bIsMoving ? SpreadOffsetCrouchWalking : SpreadOffsetCrouchIdle;
			}
			else // Standing
			{
				if (bIsMoving)
				{
					TargetOffset = bIsWalking ? SpreadOffsetWalking : SpreadOffsetRunning;
				}
			}
		}
	}

	SpreadOffset = FMath::FInterpTo(SpreadOffset, TargetOffset, DeltaTime, 10.f);
}

float AOvrlRangedWeaponInstance::GetMagnifiedFOV(float InFOV) const
{
	float TargetMagnification = 1.f;

	switch (SightMagnification)
	{
	case ESightMagnification::One:
		TargetMagnification = 1.1f; // We still want a little bit of FOV when using 1x scope
		break;
	case ESightMagnification::OneHalf:
		TargetMagnification = 1.5f;
		break;
	case ESightMagnification::Two:
		TargetMagnification = 2.f;
		break;
	case ESightMagnification::TwoHalf:
		TargetMagnification = 2.5f;
		break;
	case ESightMagnification::Three:
		TargetMagnification = 3.f;
		break;
	case ESightMagnification::Four:
		TargetMagnification = 4.f;
		break;
	case ESightMagnification::Five:
		TargetMagnification = 5.f;
		break;
	default:
		break;
	}

	return InFOV / FMath::Pow(TargetMagnification, 0.9f);
}

void AOvrlRangedWeaponInstance::PlayWeaponAnimation(UAnimSequence* AnimToPlay)
{
	if (ensure(WeaponMesh && AnimToPlay))
	{
		WeaponMesh->PlayAnimation(AnimToPlay, false);
	}
}

UMaterialInstanceDynamic* AOvrlRangedWeaponInstance::GetMagazineAmmoCountMaterial()
{
	if (!MagazineAmmoCountDisplayMat && MagazineAmmoCountDisplay && MagazineAmmoCountDisplay->GetStaticMesh())
	{
		MagazineAmmoCountDisplayMat = MagazineAmmoCountDisplay->CreateDynamicMaterialInstance(0, MagazineAmmoCountDisplay->GetMaterial(0));
	}

	return MagazineAmmoCountDisplayMat;
}

void AOvrlRangedWeaponInstance::UpdateMagazineAmmoCountDisplay()
{
	if (UMaterialInstanceDynamic* Material = GetMagazineAmmoCountMaterial())
	{
		Material->SetScalarParameterValue(TEXT("Value"), GetMagazineAmmo());
	}

	OnAmmoUpdated.Broadcast();
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
	if (ensure(WeaponMesh && OwnerSkeletalMesh))
	{
		const FTransform SocketTransform = WeaponMesh->GetSocketTransform(AimSocketName);
		FVector OutPosition;
		FRotator OutRotation;
		OwnerSkeletalMesh->TransformToBoneSpace(OwnerAttachBoneName, SocketTransform.GetLocation(), SocketTransform.GetRotation().Rotator(), OutPosition, OutRotation);

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

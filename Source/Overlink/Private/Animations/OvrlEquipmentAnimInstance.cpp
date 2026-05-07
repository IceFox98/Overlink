// Fill out your copyright notice in the Description page of Project Settings.

#include "Animations/OvrlEquipmentAnimInstance.h"

// Internal
#include "Player/OvrlCharacterBase.h"
#include "Player/Components/OvrlCharacterMovementComponent.h"
#include "Weapons/OvrlRangedWeaponInstance.h"
#include "Player/OvrlPlayerAnimInstance.h"
#include "Animations/Procedural/OvrlStanceStatesAnimManager.h"
#include "Animations/Procedural/OvrlAnimModifiers.h"
#include "Player/Components/OvrlEquipmentManagerComponent.h"
#include "OvrlUtils.h"
#include "OvrlLogUtils.h"

// Engine
#include "Animations/Procedural/OvrlAnimManagerData.h"
#include "Curves/CurveVector.h"

UOvrlEquipmentAnimInstance::UOvrlEquipmentAnimInstance()
{
	LookingSwayAlpha = 1.f;
	JumpSwayAlpha = 1.f;

	LookingSwayRotationLimit = FVector2D::One();
	LookingSwayMovementMultiplier = FVector::One();

	InitialTransformAlpha = 1.f;
}

void UOvrlEquipmentAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	CharacterMovementComponent = Cast<UOvrlCharacterMovementComponent>(PlayerCharacter->GetCharacterMovement());
}

void UOvrlEquipmentAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	check(PlayerCharacter);
	if (UOvrlEquipmentManagerComponent* EquipmentManager = PlayerCharacter->GetComponentByClass<UOvrlEquipmentManagerComponent>())
	{
		EquipmentManager->OnActiveSlotChanged.AddDynamic(this, &UOvrlEquipmentAnimInstance::OnActiveSlotChanged);
	}

	ensure(JumpSwayCurve);
	LastPlayerCameraRotation = PlayerCharacter->GetControlRotation();
	Managers.Empty();

	for (const TSoftObjectPtr<UOvrlAnimManagerData>& ManagerDataPtr : ManagersData)
	{
		if (const UOvrlAnimManagerData* ManagerData = ManagerDataPtr.LoadSynchronous())
		{
			UOvrlStanceStatesAnimManager* Manager = NewObject<UOvrlStanceStatesAnimManager>(this);
			Manager->Initialize(PlayerCharacter, ManagerData);
			Managers.Add(Manager);
		}
	}
}

void UOvrlEquipmentAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (GetParent())
	{
		SpineRotation = GetParent()->GetSpineRotation();
	}

	// Reset every frame
	RightHandInitialLocation = FVector::ZeroVector;
	RightHandInitialRotation = FRotator::ZeroRotator;

	for (UOvrlStanceStatesAnimManager* Manager : Managers)
	{
		if (Manager)
		{
			Manager->GetStartingPosition(DeltaTime, RightHandInitialLocation, RightHandInitialRotation);
		}
	}
}

void UOvrlEquipmentAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaTime)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaTime);

	if (IsValid(PlayerCharacter))
	{
		UpdateLookingSway(DeltaTime);
		UpdateJumpSway(DeltaTime);
		UpdateLeftHandIKAplha(DeltaTime);
	}
}

void UOvrlEquipmentAnimInstance::UpdateLookingSway(float DeltaTime)
{
	// Get camera delta movement, relative to the current gravity
	const FRotator LocalCameraRotation = UOvrlUtils::GetGravityRelativeRotation(PlayerCharacter->GetControlRotation(), CharacterMovementComponent->GetGravityDirection());
	const FRotator DeltaSwayRotation = UKismetMathLibrary::NormalizedDeltaRotator(LastPlayerCameraRotation, LocalCameraRotation);

	const float SwayPitch = FMath::Clamp(DeltaSwayRotation.Pitch, -LookingSwayRotationLimit.Y, LookingSwayRotationLimit.Y);
	const float SwayYaw = FMath::Clamp(-DeltaSwayRotation.Yaw, -LookingSwayRotationLimit.X, LookingSwayRotationLimit.X);
	const FRotator TargetSwayRotation = FRotator(SwayPitch, SwayYaw, 0.f);

	// Speed = 3.5
	LastLookingSwayRotation = UKismetMathLibrary::RInterpTo(LastLookingSwayRotation, TargetSwayRotation, DeltaTime, 3.5f);

	// Apply weapon sway looking to Anim BP
	LookingSwayTranslation = FVector(
		LastLookingSwayRotation.Yaw * LookingSwayMovementMultiplier.X,
		LastLookingSwayRotation.Yaw * LookingSwayMovementMultiplier.Z,
		LastLookingSwayRotation.Pitch * LookingSwayMovementMultiplier.Y
	);

	LookingSwayRotation = FRotator(
		LastLookingSwayRotation.Pitch * LookingSwayRotationMultiplier.Y,
		LastLookingSwayRotation.Yaw * LookingSwayRotationMultiplier.Z,
		LastLookingSwayRotation.Yaw * LookingSwayRotationMultiplier.X
	);

	// Save the last sway rotation
	LastPlayerCameraRotation = LocalCameraRotation;
}

void UOvrlEquipmentAnimInstance::UpdateJumpSway(float DeltaTime)
{
	if (!JumpSwayCurve)
	{
		OVRL_LOG_ERR(LogOverlink, false, "JumpSwayCurve is NULL!");
		return;
	}

	const FVector PlayerVelocity = CharacterMovementComponent->GetLastUpdateVelocity();

	// Calculate "how much" the player is jumping/falling
	// If the jump has just started, the Dot result will be equals to the JumpZVelocity, so the division will return 1.
	const float UpwardAmount = FVector::DotProduct(PlayerVelocity, PlayerCharacter->GetActorUpVector()) / CharacterMovementComponent->JumpZVelocity;
	const float RightwardAmount = FVector::DotProduct(PlayerVelocity, PlayerCharacter->GetActorRightVector()) / CharacterMovementComponent->MaxWalkSpeed;

	const float SideSwayMultiplier = .4f;

	// Apply some side sway movement, only when player jumps sideway
	const FVector TargetJumpAmount = JumpSwayCurve->GetVectorValue(-UpwardAmount) * FVector(1.f, -RightwardAmount * SideSwayMultiplier, 1.f);

	JumpSwayTranslation = UKismetMathLibrary::VectorSpringInterp(JumpSwayTranslation, TargetJumpAmount, SpringStateJump, .5f, .45f, DeltaTime, 0.005f, 5.f);

	// Calculate the rotation to apply when player jumps sideway
	const FVector JumpRotationVector = FVector(JumpSwayTranslation.Y, 0.f, JumpSwayTranslation.Y) * -JumpSwayRotationMultiplier;
	JumpSwayRotation = FRotator(0.f, JumpRotationVector.Z, JumpRotationVector.X);
}

void UOvrlEquipmentAnimInstance::UpdateLeftHandIKAplha(float DeltaTime)
{
	const bool bIsPlayerWallrunning = CharacterMovementComponent->IsWallrunning();
	const float TargetLeftHandIKAlpha = bIsPlayerWallrunning ? 0.f : 1.f;
	LeftHandIKAlpha = FMath::FInterpTo(LeftHandIKAlpha, TargetLeftHandIKAlpha, DeltaTime, 15.f);

	if (EquippedItem)
	{
		LeftHandIKTransform = EquippedItem->GetLeftHandIKTransform();
	}
}

void UOvrlEquipmentAnimInstance::OnActiveSlotChanged(const FQuickSlotEntry& ActiveSlotEntry)
{
	EquippedItem = ActiveSlotEntry.EquipmentInstance;
}

void UOvrlEquipmentAnimInstance::GetModifierValues(FGameplayTag ModifierTag, FVector& OutTranslation, FRotator& OutRotation)
{
	for (UOvrlStanceStatesAnimManager* Manager : Managers)
	{
		if (Manager)
		{
			for (UOvrlAnimModifierBase* Modifier : Manager->GetModifiers())
			{
				if (Modifier->HasTag(ModifierTag))
				{
					Modifier->Update(GetDeltaSeconds(), OutTranslation, OutRotation);
				}
			}
		}
	}

	OutTranslation = SpineRotation.RotateVector(OutTranslation);
}

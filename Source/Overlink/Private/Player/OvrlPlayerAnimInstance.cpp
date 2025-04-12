// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/OvrlPlayerAnimInstance.h"

// Internal
#include "Player/OvrlPlayerCharacter.h"	
#include "Player/Components/OvrlCharacterMovementComponent.h"
#include "Inventory/OvrlInventoryComponent.h"
#include "Weapons/OvrlRangedWeaponInstance.h"
#include "Player/Components/OvrlCameraComponent.h"
#include "OvrlUtils.h"

// Engine
#include "AbilitySystemGlobals.h"
#include "Curves/CurveVector.h"

void UOvrlPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	PlayerCharacter = Cast<AOvrlPlayerCharacter>(GetOwningActor());

#if WITH_EDITOR
	const UWorld* World = GetWorld();

	if (IsValid(World) && !World->IsGameWorld() && !IsValid(PlayerCharacter))
	{
		// Use default objects for editor preview.
		PlayerCharacter = GetMutableDefault<AOvrlPlayerCharacter>();
	}
#endif

	CharacterMovementComponent = Cast<UOvrlCharacterMovementComponent>(PlayerCharacter->GetCharacterMovement());

	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerCharacter))
	{
		GameplayTagPropertyMap.Initialize(this, ASC);
	}
}

void UOvrlPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	ensureAlways(IsValid(CharacterMovementComponent));

	PlayerCharacter->GetInventoryComponent()->OnItemEquipped.AddDynamic(this, &UOvrlPlayerAnimInstance::OnNewItemEquipped);
}

void UOvrlPlayerAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (!IsValid(CharacterMovementComponent))
		return;

	LocomotionAction = CharacterMovementComponent->GetLocomotionAction();
	LocomotionMode = CharacterMovementComponent->GetLocomotionMode();
	Stance = CharacterMovementComponent->GetStance();
	Gait = CharacterMovementComponent->GetGait();
	RightHandIKLocation = CharacterMovementComponent->GetRightHandIKLocation();
	LeftHandIKLocation = CharacterMovementComponent->GetLeftHandIKLocation();
}

void UOvrlPlayerAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaTime)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaTime);

	if (IsValid(EquippedWeapon))
	{
		WeaponRecoil = EquippedWeapon->GetWeaponKickbackRecoil();
		WeaponCameraRecoil = EquippedWeapon->GetWeaponCameraRecoil();
		WeaponAimTransform = EquippedWeapon->GetAimTransform();

		UpdateWeaponSway(DeltaTime);
	}
	// TODO: Else to check if melee weapon?
}

void UOvrlPlayerAnimInstance::UpdateWeaponSway(float DeltaTime)
{
	// Sway applied when moving the camera around (mouse movement)
	UpdateWeaponSwayLooking(DeltaTime);

	// Manage sway movement
	const FVector DeltaSwayMovement = CharacterMovementComponent->GetRelativeLastUpdateVelocity();

	const float SwayX = FMath::Clamp(DeltaSwayMovement.X, -EquippedWeapon->WeaponSwayMovementLimit.X, EquippedWeapon->WeaponSwayMovementLimit.X);
	const float SwayY = FMath::Clamp(DeltaSwayMovement.Y, -EquippedWeapon->WeaponSwayMovementLimit.Y, EquippedWeapon->WeaponSwayMovementLimit.Y);
	const float SwayZ = FMath::Clamp(DeltaSwayMovement.Z, -1.f, EquippedWeapon->WeaponSwayMovementLimit.Z);

	const FVector Test = EquippedWeapon->GetWalkSwayCurve()->GetVectorValue(GetWorld()->GetTimeSeconds());

	const FVector TargetSwayMovement = FVector(SwayY, -SwayX, SwayZ);
	//const FVector TargetSwayMovement = EquippedWeapon->GetWalkSwayCurve()->GetVectorValue(GetWorld()->GetTimeSeconds());
	
	//WeaponSwayMovement = FMath::VInterpTo(WeaponSwayMovement, TargetSwayMovement, DeltaTime, EquippedWeapon->WeaponSwayMovementSpeed);
	WeaponSwayMovement = UKismetMathLibrary::VectorSpringInterp(WeaponSwayMovement, TargetSwayMovement, SpringStateMovement, .4f, .3, DeltaTime, 0.006f);

	//OVRL_LOG("Delta: %s - Current: %s - Target: %s", *DeltaSwayMovement.ToString(), *WeaponSwayMovement.ToString(), *TargetSwayMovement.ToString());

	WeaponSwayMovementPrev = CharacterMovementComponent->GetLastUpdateVelocity();
}

void UOvrlPlayerAnimInstance::UpdateWeaponSwayLooking(float DeltaTime)
{
	// Get camera delta movement
	const FRotator DeltaSwayRotation = UKismetMathLibrary::NormalizedDeltaRotator(WeaponSwayRotationPrev, PlayerCharacter->GetCameraComponent()->GetComponentRotation());

	const float SwayPitch = FMath::Clamp(DeltaSwayRotation.Pitch, -EquippedWeapon->WeaponSwayRotationLimit.Y, EquippedWeapon->WeaponSwayRotationLimit.Y);
	const float SwayYaw = FMath::Clamp(-DeltaSwayRotation.Yaw, -EquippedWeapon->WeaponSwayRotationLimit.X, EquippedWeapon->WeaponSwayRotationLimit.X);
	const FRotator TargetSwayRotation = FRotator(SwayPitch, SwayYaw, 0.f);

	WeaponSwayRotation = UKismetMathLibrary::QuaternionSpringInterp(FQuat(WeaponSwayRotation), FQuat(TargetSwayRotation), SpringStateRotation, .5f, .8f, DeltaTime, 0.006f).Rotator();
	//WeaponSwayRotation = FMath::RInterpTo(WeaponSwayRotation, TargetSwayRotation, DeltaTime, EquippedWeapon->WeaponSwayRotationSpeed);

	WeaponSwayRotationPrev = PlayerCharacter->GetCameraComponent()->GetComponentRotation();

	// Apply weapon sway looking to Anim BP
	WeaponSwayLooking = FVector(0.f, WeaponSwayRotation.Yaw, WeaponSwayRotation.Pitch);
}

void UOvrlPlayerAnimInstance::OnNewItemEquipped(AOvrlEquipmentInstance* EquippedItem)
{
	if (EquippedItem->IsA<AOvrlRangedWeaponInstance>())
	{
		EquippedWeapon = Cast<AOvrlRangedWeaponInstance>(EquippedItem);
	}
	else
	{
		EquippedWeapon = nullptr;
	}
}

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
	//OverlayMode = CharacterMovementComponent->GetOverlayMode();
	RightHandIKLocation = CharacterMovementComponent->GetRightHandIKLocation();
	LeftHandIKLocation = CharacterMovementComponent->GetLeftHandIKLocation();
}

void UOvrlPlayerAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaTime)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaTime);

	//const FBasedMovementInfo& BasedMovement = Character->GetBasedMovement();
	if (IsValid(EquippedWeapon))
	{
		WeaponRecoil = EquippedWeapon->GetWeaponKickbackRecoil();
		WeaponCameraRecoil = EquippedWeapon->GetWeaponCameraRecoil();

		UpdateWeaponSway(DeltaTime);
	}
}

void UOvrlPlayerAnimInstance::UpdateWeaponSway(float DeltaTime)
{
	// Manage sway rotation
	const FRotator DeltaSwayRotation = WeaponSwayRotationPrev - PlayerCharacter->GetCameraComponent()->GetComponentRotation();

	const float SwayPitch = FMath::Clamp(DeltaSwayRotation.Pitch, -EquippedWeapon->WeaponSwayRotationLimit.Y, EquippedWeapon->WeaponSwayRotationLimit.Y);
	const float SwayYaw = FMath::Clamp(-DeltaSwayRotation.Yaw, -EquippedWeapon->WeaponSwayRotationLimit.X, EquippedWeapon->WeaponSwayRotationLimit.X);

	const FRotator TargetSwayRotation = FRotator(SwayPitch, SwayYaw, 0.f);

	WeaponSwayRotation = FMath::RInterpTo(WeaponSwayRotation, TargetSwayRotation, DeltaTime, EquippedWeapon->WeaponSwayRotationSpeed);
	WeaponSwayRotationPrev = PlayerCharacter->GetCameraComponent()->GetComponentRotation();

	// Manage sway movement
	const FVector DeltaSwayMovement = CharacterMovementComponent->GetLastInputVector();

	OVRL_LOG("%s", *DeltaSwayMovement.ToString());

	const float SwayX = FMath::Clamp(DeltaSwayMovement.X, -EquippedWeapon->WeaponSwayMovementLimit.X, EquippedWeapon->WeaponSwayMovementLimit.X);
	const float SwayY = FMath::Clamp(DeltaSwayMovement.Y, -EquippedWeapon->WeaponSwayMovementLimit.Y, EquippedWeapon->WeaponSwayMovementLimit.Y);
	const float SwayZ = FMath::Clamp(DeltaSwayMovement.Z, -EquippedWeapon->WeaponSwayMovementLimit.Z, EquippedWeapon->WeaponSwayMovementLimit.Z);

	const FVector TargetSwayMovement = FVector(SwayX, SwayY, SwayZ);
	//WeaponSwayMovement = FMath::VInterpTo(WeaponSwayMovement, TargetSwayMovement, DeltaTime, EquippedWeapon->WeaponSwayMovementSpeed);

	WeaponSwayMovement = UKismetMathLibrary::VectorSpringInterp(WeaponSwayMovement, TargetSwayMovement, SpringState, .4f, .6, DeltaTime, 0.006f);

	WeaponSwayMovementPrev = CharacterMovementComponent->GetLastUpdateVelocity();
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

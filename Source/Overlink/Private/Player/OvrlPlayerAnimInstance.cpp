// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/OvrlPlayerAnimInstance.h"

// Internal
#include "Player/OvrlPlayerCharacter.h"	
#include "Player/Components/OvrlCharacterMovementComponent.h"
#include "Inventory/OvrlInventoryComponent.h"
#include "Weapons/OvrlRangedWeaponInstance.h"

// Engine
#include "AbilitySystemGlobals.h"

void UOvrlPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	PlayerCharacter = Cast<AOvrlPlayerCharacter>(GetOwningActor());

#if WITH_EDITOR
	const UWorld* World = GetWorld() ;

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

	if (AOvrlRangedWeaponInstance* WeaponInstance = Cast<AOvrlRangedWeaponInstance>(PlayerCharacter->GetInventoryComponent()->GetEquippedItem()))
	{
		WeaponRecoil = WeaponInstance->GetWeaponKickbackRecoil();
		WeaponCameraRecoil = WeaponInstance->GetWeaponCameraRecoil();

	}
}

void UOvrlPlayerAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaTime)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaTime);

	//const FBasedMovementInfo& BasedMovement = Character->GetBasedMovement();

}

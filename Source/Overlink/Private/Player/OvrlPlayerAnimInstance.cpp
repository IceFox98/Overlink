// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/OvrlPlayerAnimInstance.h"

// Internal
#include "Player/OvrlPlayerCharacter.h"	
//#include "AbilitySystem/OvrlAbilitySystemComponent.h"

// Engine
#include "AbilitySystemGlobals.h"

void UOvrlPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<AOvrlPlayerCharacter>(GetOwningActor());

#if WITH_EDITOR
	const auto* World{ GetWorld() };

	if (IsValid(World) && !World->IsGameWorld() && !IsValid(Character))
	{
		// Use default objects for editor preview.
		Character = GetMutableDefault<AOvrlPlayerCharacter>();
	}
#endif

	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Character))
	{
		GameplayTagPropertyMap.Initialize(this, ASC);
	}
}

void UOvrlPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	ensure(IsValid(Character));
}

void UOvrlPlayerAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

}

void UOvrlPlayerAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaTime)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaTime);

	const FBasedMovementInfo& BasedMovement = Character->GetBasedMovement();

	LocomotionMode = Character->GetLocomotionMode();
	Stance = Character->GetStance();
	Gait = Character->GetGait();
	OverlayMode = Character->GetOverlayMode();
}

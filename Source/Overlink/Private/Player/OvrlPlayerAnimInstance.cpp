// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/OBM_PlayerAnimInstance.h"

#include "Player/OBM_PlayerCharacter.h"	

void UOBM_PlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Character = Cast<AOBM_PlayerCharacter>(GetOwningActor());

#if WITH_EDITOR
	const auto* World{ GetWorld() };

	if (IsValid(World) && !World->IsGameWorld() && !IsValid(Character))
	{
		// Use default objects for editor preview.
		Character = GetMutableDefault<AOBM_PlayerCharacter>();
	}
#endif
}

void UOBM_PlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	ensure(IsValid(Character));
}

void UOBM_PlayerAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

}

void UOBM_PlayerAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaTime)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaTime);

	const FBasedMovementInfo& BasedMovement = Character->GetBasedMovement();

	LocomotionMode = Character->GetLocomotionMode();
	Stance = Character->GetStance();
	Gait = Character->GetGait();
	OverlayMode = Character->GetOverlayMode();
}

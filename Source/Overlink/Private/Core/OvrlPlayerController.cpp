// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/OvrlPlayerController.h"
#include "Core/OvrlPlayerCameraManager.h"
#include "Player/OvrlCharacterBase.h"
#include "Player/Components/OvrlCharacterMovementComponent.h"
#include "AbilitySystem/OvrlAbilitySystemComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"

#include "OvrlUtils.h"

AOvrlPlayerController::AOvrlPlayerController()
{
	PlayerCameraManagerClass = AOvrlPlayerCameraManager::StaticClass();
}

void AOvrlPlayerController::BeginPlay()
{
	Super::BeginPlay();

	OwningCharacter = Cast<AOvrlCharacterBase>(GetCharacter());
	ensure(OwningCharacter);

	CharacterMovementComponent = Cast<UOvrlCharacterMovementComponent>(OwningCharacter->GetCharacterMovement());
	ensure(CharacterMovementComponent);
}

void AOvrlPlayerController::UpdateRotation(float DeltaTime)
{
	//Super::UpdateRotation(DeltaTime);
	//return;

	if (!CharacterMovementComponent)
	{
		return;
	}

	FVector GravityDirection = CharacterMovementComponent->GetGravityDirection();

	// Get the current control rotation in world space
	FRotator ViewRotation = GetControlRotation();

	// Convert the view rotation from world space to gravity relative space.
	// Now we can work with the rotation as if no custom gravity was affecting it.
	ViewRotation = UOvrlUtils::GetGravityRelativeRotation(ViewRotation, GravityDirection);

	LastRotationInput = RotationInput;

	// Calculate Delta to be applied on ViewRotation
	FRotator DeltaRot(RotationInput);

	if (PlayerCameraManager)
	{
		double TargetRoll = 0;

		if (CharacterMovementComponent->IsLateralWallrunning())
		{
			TargetRoll = CharacterMovementComponent->LocomotionAction == OvrlLocomotionActionTags::WallrunningLeft ? CharacterMovementComponent->WallrunCameraTiltAngle : -CharacterMovementComponent->WallrunCameraTiltAngle;
		}

		// Add Delta Rotation
		ViewRotation += DeltaRot;

		// Limit Player View Axes (skip Roll to avoid rotation optimizations)
		PlayerCameraManager->LimitViewPitch(ViewRotation, PlayerCameraManager->ViewPitchMin, PlayerCameraManager->ViewPitchMax);
		PlayerCameraManager->LimitViewYaw(ViewRotation, PlayerCameraManager->ViewYawMin, PlayerCameraManager->ViewYawMax);

		ViewRotation.Roll = UKismetMathLibrary::FInterpTo(ViewRotation.Roll, TargetRoll, DeltaTime, 10.f);

		// Convert the rotation back to world space, and set it as the current control rotation.
		SetControlRotation(UOvrlUtils::GetGravityWorldRotation(ViewRotation, GravityDirection));
	}

	APawn* const P = GetPawnOrSpectator();
	if (P)
	{
		P->FaceRotation(ViewRotation, DeltaTime);
	}
}

void AOvrlPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (ensure(OwningCharacter))
	{
		if (UOvrlAbilitySystemComponent* OvrlASC = OwningCharacter->GetOvrlAbilitySystemComponent())
		{
			OvrlASC->ProcessAbilityInput(DeltaTime, bGamePaused);
		}
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

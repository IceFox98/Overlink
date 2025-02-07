// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/OvrlPlayerController.h"
#include "Player/OvrlPlayerCharacter.h"
#include "Player/Components/OvrlParkourComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "OvrlUtils.h"

void AOvrlPlayerController::UpdateRotation(float DeltaTime)
{
	//Super::UpdateRotation(DeltaTime);
	//return;

	FVector GravityDirection = FVector::DownVector;
	AOvrlPlayerCharacter* PlayerCharacter = Cast<AOvrlPlayerCharacter>(GetPawn());

	if (PlayerCharacter)
	{
		if (UCharacterMovementComponent* MoveComp = PlayerCharacter->GetCharacterMovement())
		{
			GravityDirection = MoveComp->GetGravityDirection();
		}
	}

	// Get the current control rotation in world space
	FRotator ViewRotation = GetControlRotation();

	// Convert the view rotation from world space to gravity relative space.
	// Now we can work with the rotation as if no custom gravity was affecting it.
	ViewRotation = UOvrlUtils::GetGravityRelativeRotation(ViewRotation, GravityDirection);

	// Calculate Delta to be applied on ViewRotation
	FRotator DeltaRot(RotationInput);

	if (PlayerCameraManager)
	{
		double TargetRoll = 0;

		if (const UOvrlParkourComponent* ParkourComponent = PlayerCharacter->GetParkourComponent())
		{
			if (ParkourComponent->IsWallrunning())
			{
				TargetRoll = ParkourComponent->GetMovementType() == EParkourMovementType::WallrunLeft ? ParkourComponent->WallrunCameraTiltAngle : -ParkourComponent->WallrunCameraTiltAngle;
			}
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
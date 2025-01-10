// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/OBM_PlayerController.h"
#include "Player/PlayerCharacter.h"
#include "Player/Components/ParkourComponent.h"

#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "OBM_Utils.h"

void AOBM_PlayerController::UpdateRotation(float DeltaTime)
{
	//Super::UpdateRotation(DeltaTime);
	//return;

	FVector GravityDirection = FVector::DownVector;
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetPawn());

	if (PlayerCharacter)
	{
		if (UCharacterMovementComponent* MoveComp = PlayerCharacter->GetCharacterMovement())
		{
			GravityDirection = MoveComp->GetGravityDirection();
		}
	}

	// Get the current control rotation in world space
	FRotator ViewRotation = GetControlRotation();

	//// Add any rotation from the gravity changes, if any happened.
	//// Delete this code block if you don't want the camera to automatically compensate for gravity rotation.
	//if (!LastFrameGravity.Equals(FVector::ZeroVector))
	//{
	//	const FQuat DeltaGravityRotation = FQuat::FindBetweenNormals(LastFrameGravity, GravityDirection);
	//	const FQuat WarpedCameraRotation = DeltaGravityRotation * FQuat(ViewRotation);

	//	ViewRotation = WarpedCameraRotation.Rotator();
	//}
	//LastFrameGravity = GravityDirection;

	// Convert the view rotation from world space to gravity relative space.
	// Now we can work with the rotation as if no custom gravity was affecting it.
	ViewRotation = UOBM_Utils::GetGravityRelativeRotation(ViewRotation, GravityDirection);

	// Calculate Delta to be applied on ViewRotation
	FRotator DeltaRot(RotationInput);

	if (PlayerCameraManager)
	{
		double TargetRoll = 0;

		if (const UParkourComponent* ParkourComponent = PlayerCharacter->GetParkourComponent())
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
		SetControlRotation(UOBM_Utils::GetGravityWorldRotation(ViewRotation, GravityDirection));
	}

	APawn* const P = GetPawnOrSpectator();
	if (P)
	{
		P->FaceRotation(ViewRotation, DeltaTime);
	}
}
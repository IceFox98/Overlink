// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Components/OBM_CameraComponent.h"

#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"

void UOBM_CameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	UpdateForOwner();
	UpdateCrouchOffset(DeltaTime);

	FVector PivotLocation = GetPivotLocation() + CurrentCrouchOffset;
	FRotator PivotRotation = GetPivotRotation();

	// Update current camera view, without moving the actual camera object
	DesiredView.Location = PivotLocation;
	DesiredView.Rotation = PivotRotation;
}

FVector UOBM_CameraComponent::GetPivotLocation() const
{
	const AActor* TargetActor = GetOwner();
	check(TargetActor);

	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		// Height adjustments for characters to account for crouching.
		if (const ACharacter* TargetCharacter = Cast<ACharacter>(TargetPawn))
		{
			const ACharacter* TargetCharacterCDO = TargetCharacter->GetClass()->GetDefaultObject<ACharacter>();
			check(TargetCharacterCDO);

			const UCapsuleComponent* CapsuleComp = TargetCharacter->GetCapsuleComponent();
			check(CapsuleComp);

			const UCapsuleComponent* CapsuleCompCDO = TargetCharacterCDO->GetCapsuleComponent();
			check(CapsuleCompCDO);

			const float DefaultHalfHeight = CapsuleCompCDO->GetUnscaledCapsuleHalfHeight();
			const float ActualHalfHeight = CapsuleComp->GetUnscaledCapsuleHalfHeight();
			const float HeightAdjustment = (DefaultHalfHeight - ActualHalfHeight) + TargetCharacterCDO->BaseEyeHeight;

			return TargetCharacter->GetActorLocation() + (FVector::UpVector * HeightAdjustment);
		}

		return TargetPawn->GetPawnViewLocation();
	}

	return TargetActor->GetActorLocation();
}

FRotator UOBM_CameraComponent::GetPivotRotation() const
{
	const AActor* TargetActor = GetOwner();
	check(TargetActor);

	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		return TargetPawn->GetViewRotation();
	}

	return TargetActor->GetActorRotation();
}

void UOBM_CameraComponent::UpdateForOwner()
{
	if (const ACharacter* TargetCharacter = Cast<ACharacter>(GetOwner()))
	{
		if (TargetCharacter->bIsCrouched)
		{
			// Get Class Default Object (CDO) of the player
			const ACharacter* TargetCharacterCDO = TargetCharacter->GetClass()->GetDefaultObject<ACharacter>();

			// Get the offset between base and crouched eye position
			const float CrouchedHeightAdjustment = TargetCharacterCDO->CrouchedEyeHeight - TargetCharacterCDO->BaseEyeHeight;

			SetTargetCrouchOffset(FVector(0.f, 0.f, CrouchedHeightAdjustment));

			return;
		}
	}

	SetTargetCrouchOffset(FVector::ZeroVector);
}

void UOBM_CameraComponent::SetTargetCrouchOffset(FVector NewTargetOffset)
{
	CrouchOffsetBlendAlpha = 0.f;
	InitialCrouchOffset = CurrentCrouchOffset;
	TargetCrouchOffset = NewTargetOffset;
}

void UOBM_CameraComponent::UpdateCrouchOffset(float DeltaTime)
{
	if (CrouchOffsetBlendAlpha < 1.0f)
	{
		// Add blend multiplier times DeltaTime to CrouchOffsetBlendAlpha
		CrouchOffsetBlendAlpha = FMath::Min(CrouchOffsetBlendAlpha + DeltaTime * CrouchOffsetBlendMultiplier, 1.0f);

		// Interpolate the initial crouch offset with target one, slowing down start (EaseIn) and end (EaseOut)
		CurrentCrouchOffset = FMath::InterpEaseInOut(InitialCrouchOffset, TargetCrouchOffset, CrouchOffsetBlendAlpha, 1.0f);
	}
	else
	{
		CurrentCrouchOffset = TargetCrouchOffset;
		CrouchOffsetBlendAlpha = 1.0f;
	}
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Components/OvrlCameraComponent.h"

#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void UOvrlCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	Super::GetCameraView(DeltaTime, DesiredView);

	//return;

	UpdateForOwner();
	UpdateCrouchOffset(DeltaTime);

	FVector PivotLocation = GetPivotLocation() + CurrentCrouchOffset;
	//FRotator PivotRotation = GetPivotRotation();

	// Update current camera view, without moving the actual camera object
	//DesiredView.Location = PivotLocation;
	//DesiredView.Rotation = PivotRotation;
}

FVector UOvrlCameraComponent::GetPivotLocation() const
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

			const UCharacterMovementComponent* MoveComp = TargetCharacter->GetCharacterMovement();
			check(MoveComp);

			const float DefaultHalfHeight = CapsuleCompCDO->GetUnscaledCapsuleHalfHeight();
			const float ActualHalfHeight = CapsuleComp->GetUnscaledCapsuleHalfHeight();
			const float HeightAdjustment = (DefaultHalfHeight - ActualHalfHeight) + TargetCharacterCDO->BaseEyeHeight;

			return TargetCharacter->GetActorLocation() + (-MoveComp->GetGravityDirection() * HeightAdjustment);
		}

		return TargetPawn->GetPawnViewLocation();
	}

	return TargetActor->GetActorLocation();
}

FRotator UOvrlCameraComponent::GetPivotRotation() const
{
	const AActor* TargetActor = GetOwner();
	check(TargetActor);

	if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
	{
		return TargetPawn->GetViewRotation();
	}

	return TargetActor->GetActorRotation();
}

void UOvrlCameraComponent::UpdateForOwner()
{
	if (const ACharacter* TargetCharacter = Cast<ACharacter>(GetOwner()))
	{
		if (TargetCharacter->bIsCrouched)
		{
			// Get Class Default Object (CDO) of the player
			const ACharacter* TargetCharacterCDO = TargetCharacter->GetClass()->GetDefaultObject<ACharacter>();

			// Get the offset between base and crouched eye position
			const float CrouchedHeightAdjustment = TargetCharacterCDO->CrouchedEyeHeight - TargetCharacterCDO->BaseEyeHeight;

			const UCharacterMovementComponent* MoveComp = TargetCharacter->GetCharacterMovement();

			//SetTargetCrouchOffset(FVector(0.f, 0.f, CrouchedHeightAdjustment));
			SetTargetCrouchOffset(-MoveComp->GetGravityDirection().GetSafeNormal() * CrouchedHeightAdjustment);

			return;
		}
	}

	SetTargetCrouchOffset(FVector::ZeroVector);
}

void UOvrlCameraComponent::SetTargetCrouchOffset(FVector NewTargetOffset)
{
	CrouchOffsetBlendAlpha = 0.f;
	InitialCrouchOffset = CurrentCrouchOffset;
	TargetCrouchOffset = NewTargetOffset;
}

void UOvrlCameraComponent::UpdateCrouchOffset(float DeltaTime)
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


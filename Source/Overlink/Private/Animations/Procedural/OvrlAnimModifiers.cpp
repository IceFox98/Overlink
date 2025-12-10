// Fill out your copyright notice in the Description page of Project Settings.

#include "Animations/Procedural/OvrlAnimModifiers.h"
#include "Animations/Procedural/OvrlAnimAlphaModifiers.h"

#include "Player/OvrlPlayerCharacter.h"
#include "Player/Components/OvrlCharacterMovementComponent.h"

// Engine
#include "Curves/CurveVector.h"

void UOvrlAnimModifierBase::Initialize(AOvrlPlayerCharacter* InPlayerCharacter)
{
	PlayerCharacter = InPlayerCharacter;
	CharacterMovementComponent = Cast<UOvrlCharacterMovementComponent>(PlayerCharacter->GetCharacterMovement());

	for (UOvrlAnimAlphaModifierBase* AlphaModifier : AlphaModifiers)
	{
		if (AlphaModifier)
		{
			AlphaModifier->Initialize();
		}
	}
}

void UOvrlAnimModifierBase::Toggle(bool bEnable)
{
	bShouldUpdateAlpha = bEnable;

	// TODO: Create a second varible for the Recovery speed? To have different values during "disabling event"
}

void UOvrlAnimModifierBase::Update(float DeltaTime, FVector& OutTranslation, FRotator& OutRotation)
{
	ComputeAlpha(DeltaTime);

	if (Alpha > 0.f && bEnabled)
	{
		UpdateImpl(DeltaTime, OutTranslation, OutRotation);
	}
}

void UOvrlAnimModifierBase::SetTag(const FGameplayTag& NewTag)
{
	if (CurrentTag != NewTag)
	{
		for (FModifierData& Data : DataList)
		{
			Data.Time = 0.f;
		}
	}

	CurrentTag = NewTag;
}

void UOvrlAnimModifierBase::ComputeAlpha(float DeltaTime)
{
	if (TagsToCheck.HasTagExact(CurrentTag) && bShouldUpdateAlpha)
	{
		Alpha = 1.f; // We want fully alpha to be applied
	}
	else if (Alpha <= KINDA_SMALL_NUMBER)
	{
		Alpha = 0.f;
	}
	else
	{
		// Smoothly decrease alpha, to avoid jerky movements
		Alpha = FMath::FInterpTo(Alpha, 0.f, DeltaTime, RecoverySpeed);
	}
}

float UOvrlAnimModifierBase::GetAlpha()
{
	float TargetAlpha = Alpha;

	for (UOvrlAnimAlphaModifierBase* AlphaModifier : AlphaModifiers)
	{
		if (AlphaModifier)
		{
			AlphaModifier->ModifyAlpha(TargetAlpha);
		}
	}

	return TargetAlpha;
}

void UOvrlSimpleAnimModifier::UpdateImpl(float DeltaTime, FVector& OutTranslation, FRotator& OutRotation)
{
	const float TargetAlpha = GetAlpha();

	for (FModifierData& Data : DataList)
	{
		if (!Data.TranslationCurve || !Data.RotationCurve)
		{
			continue;
		}

		// Apply translation curve, modified by the movement amount
		OutTranslation += Data.TranslationCurve->GetVectorValue(Data.Time) * Data.TranslationMultiplier * TargetAlpha;

		// Apply rotation curve
		const FVector RotationCurve = Data.RotationCurve->GetVectorValue(Data.Time) * Data.RotationMultiplier * TargetAlpha;
		OutRotation += FRotator(RotationCurve.Y, RotationCurve.Z, RotationCurve.X);

		// Increase time of this modifier data
		Data.Time += DeltaTime * Data.Frequency;
	}
}

void UOvrlLocomotionActionsAnimModifier::UpdateImpl(float DeltaTime, FVector& OutTranslation, FRotator& OutRotation)
{
	FVector TargetTranslation = FVector::ZeroVector;
	FRotator TargetRotation = FRotator::ZeroRotator;

	const FVector LastInputVector = CharacterMovementComponent->GetLastInputVector();

	// Read the value only when player moves
	if (LastInputVector.Length() > 0.f)
	{
		const float ForwardAmount = FVector::DotProduct(LastInputVector, PlayerCharacter->GetActorForwardVector());
		const float RightwardAmount = FVector::DotProduct(LastInputVector, PlayerCharacter->GetActorRightVector());

		// Apply a list of transformation in order to handle behaviors like starting movement
		for (FModifierData& Data : DataList)
		{
			if (!Data.TranslationCurve || !Data.RotationCurve)
			{
				continue;
			}

			// Apply translation curve, modified by the movement amount
			TargetTranslation += Data.TranslationCurve->GetVectorValue(Data.Time) * Data.TranslationMultiplier * FVector(1.f, ForwardAmount, 1.f);

			// Apply rotation curve
			const FVector RotationCurve = Data.RotationCurve->GetVectorValue(Data.Time) * Data.RotationMultiplier;
			TargetRotation += FRotator(RotationCurve.Y, RotationCurve.Z, RotationCurve.X);

			// Increase time of this modifier data
			Data.Time += DeltaTime * Data.Frequency;
		}
	}
	else
	{
		for (FModifierData& Data : DataList)
		{
			Data.Time = 0.f;
		}
	}

	const float TargetAlpha = GetAlpha();

	// Since OutTranslation and OutRotation is reset every frame, we save LastTranslation and LastRotation locally to be able to smooth correctly
	LastTranslation = FMath::VInterpTo(LastTranslation, TargetTranslation, DeltaTime, InterpSpeed) * TargetAlpha;
	OutTranslation += LastTranslation;

	LastRotation = FMath::RInterpTo(LastRotation, TargetRotation, DeltaTime, InterpSpeed) * TargetAlpha;
	OutRotation += LastRotation;
}

void UOvrlMovementAnimModifier::UpdateImpl(float DeltaTime, FVector& OutTranslation, FRotator& OutRotation)
{
	const FVector PlayerVelocity = CharacterMovementComponent->GetLastUpdateVelocity();

	// Calculate "how much" the player is moving in any directions
	// If it's moving fully forward, the Dot result will be equals to the MaxWalkSpeed, so the division will return 1.
	const float ForwardAmount = FVector::DotProduct(PlayerVelocity, PlayerCharacter->GetActorForwardVector()) / CharacterMovementComponent->MaxWalkSpeed;
	const float RightwardAmount = FVector::DotProduct(PlayerVelocity, PlayerCharacter->GetActorRightVector()) / CharacterMovementComponent->MaxWalkSpeed;

	const float TargetAlpha = GetAlpha();
	FVector TargetMovementAmount = FVector::ZeroVector;
	FRotator TargetRotation = FRotator::ZeroRotator;

	for (FModifierData& Data : DataList)
	{
		TargetMovementAmount += FVector(RightwardAmount, -ForwardAmount, 0.f) * Data.TranslationMultiplier;
	}

	LastTranslation = FMath::VInterpTo(LastTranslation, TargetMovementAmount, DeltaTime, InterpSpeed) * TargetAlpha;

	for (FModifierData& Data : DataList)
	{
		OutRotation += FRotator(FMath::Clamp(LastTranslation.X, -1.f, 1.f) * -Data.RotationMultiplier.X, 0.f, 0.f);
	}

	// Since OutTranslation and OutRotation is reset every frame, we save LastTranslation and LastRotation locally to be able to smooth correctly
	OutTranslation += LastTranslation;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CameraModifiers/OvrlCameraModifierBase.h"

// Engine
#include "Camera/CameraAnimationHelper.h"
#include "Curves/CurveVector.h"

UOvrlCameraModifierBase::UOvrlCameraModifierBase()
{
	AlphaInTime = 1.f;
	AlphaOutTime = 1.f;
}

bool UOvrlCameraModifierBase::ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutPOV)
{
	Super::ModifyCamera(DeltaTime, InOutPOV);

	FVector OutLocation = FVector::ZeroVector;
	if (TranslationCurve)
	{
		// Apply translation curve, modified by the movement amount
		OutLocation = TranslationCurve->GetVectorValue(ElapsedTime) * TranslationMultiplier * Alpha;
	}

	FRotator OutRotation = FRotator::ZeroRotator;
	if (RotationCurve)
	{
		// Apply rotation curve
		const FVector RotationCurveValue = RotationCurve->GetVectorValue(ElapsedTime) * RotationMultiplier * Alpha;
		OutRotation = FRotator(RotationCurveValue.Y, RotationCurveValue.Z, RotationCurveValue.X);
	}

	float FOVOffset = 0.f;
	if (FOVModifier)
	{
		FOVOffset = FOVModifier->GetFloatValue(ElapsedTime) * Alpha;
	}

	const FCameraAnimationHelperOffset CameraOffset{ OutLocation, OutRotation };
	FCameraAnimationHelper::ApplyOffset(InOutPOV, CameraOffset, OutLocation, OutRotation);

	InOutPOV.Location = OutLocation;
	InOutPOV.Rotation = OutRotation;
	InOutPOV.FOV += FOVOffset;

	ElapsedTime += DeltaTime * Frequency;

	if (AutoDisableAfterSeconds > KINDA_SMALL_NUMBER)
	{
		if (!bPendingDisable && ElapsedTime >= AutoDisableAfterSeconds)
		{
			DisableModifier(true);
			return false;
		}
	}

	// Return false to allow other modifiers to play (camera shakes)
	return false;
}

void UOvrlCameraModifierBase::DisableModifier(bool bImmediate)
{
	Super::DisableModifier(bImmediate);

	if (bImmediate)
	{
		ElapsedTime = 0.f;
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CameraModifiers/OvrlCameraModifier_FOV.h"

UOvrlCameraModifier_FOV::UOvrlCameraModifier_FOV()
{
	AlphaInTime = 1.f;
	AlphaOutTime = 1.f;
}

bool UOvrlCameraModifier_FOV::ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutPOV)
{
	Super::ModifyCamera(DeltaTime, InOutPOV);

	// If this is the first frame, initialize CurrentFOV
	if (CurrentFOV == 0.f)
	{
		CurrentFOV = InOutPOV.FOV;
	}

	// Interpolate smoothly toward target
	CurrentFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, InterpSpeed);

	InOutPOV.FOV = CurrentFOV;

	// Return false to allow other modifiers to play (camera shakes)
	return false;
}

void UOvrlCameraModifier_FOV::DisableModifier(bool bImmediate)
{
	Super::DisableModifier(bImmediate);

	if (CameraOwner)
	{
		TargetFOV = CameraOwner->DefaultFOV;
	}
}

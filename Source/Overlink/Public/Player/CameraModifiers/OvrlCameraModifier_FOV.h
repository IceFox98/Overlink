// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraModifier.h"
#include "OvrlCameraModifier_FOV.generated.h"

/**
 * 
 */
UCLASS()
class OVERLINK_API UOvrlCameraModifier_FOV : public UCameraModifier
{
	GENERATED_BODY()

public:

	UOvrlCameraModifier_FOV();

public:

	virtual bool ModifyCamera(float DeltaTime, struct FMinimalViewInfo& InOutPOV) override;
	virtual void DisableModifier(bool bImmediate = false) override;

	void SetTargetFOV(float InTargetFOV) { TargetFOV = InTargetFOV; };
	void SetInterpSpeed(float InNewSpeed) { InterpSpeed = InNewSpeed; };

private:

	float InterpSpeed = 0.f;
	float TargetFOV;
	float CurrentFOV;
};

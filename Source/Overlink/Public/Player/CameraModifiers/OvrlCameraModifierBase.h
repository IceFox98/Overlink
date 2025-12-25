// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraModifier.h"
#include "OvrlCameraModifierBase.generated.h"

class UOvrlSimpleAnimModifier;
class UCurveVector;

/**
 * 
 */
UCLASS()
class OVERLINK_API UOvrlCameraModifierBase : public UCameraModifier
{
	GENERATED_BODY()

public:

	UOvrlCameraModifierBase();

public:

	virtual bool ModifyCamera(float DeltaTime, struct FMinimalViewInfo& InOutPOV) override;
	virtual void DisableModifier(bool bImmediate = false) override;

public:

	UPROPERTY(EditAnywhere)
	float Frequency = 1.f;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UCurveVector> TranslationCurve;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UCurveVector> RotationCurve;

	UPROPERTY(EditAnywhere)
	FVector TranslationMultiplier;

	UPROPERTY(EditAnywhere)
	FVector RotationMultiplier;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "FOV Modifier"))
	TObjectPtr<UCurveFloat> FOVModifier;

	// If > 0, this modifier will auto disable after N seconds
	UPROPERTY(EditAnywhere)
	float AutoDisableAfterSeconds = 0.f;

private:

	float ElapsedTime = 0.f;
};

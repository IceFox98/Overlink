// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "OBM_CameraComponent.generated.h"

/**
 * 
 */
UCLASS()
class OVERLINK_API UOBM_CameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:

	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;

protected:

	FVector GetPivotLocation() const;
	FRotator GetPivotRotation() const;

private:

	void UpdateForOwner();
	void SetTargetCrouchOffset(FVector NewTargetOffset);
	void UpdateCrouchOffset(float DeltaTime);

protected:

	// Alters the speed that a crouch offset is blended in or out
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OBM|Camera")
		float CrouchOffsetBlendMultiplier = 5.0f;

private:

	// Controls the "alpha" of the blend ease
	float CrouchOffsetBlendAlpha = 1.0f;

	FVector InitialCrouchOffset = FVector::ZeroVector;
	FVector TargetCrouchOffset = FVector::ZeroVector;
	FVector CurrentCrouchOffset = FVector::ZeroVector;
};

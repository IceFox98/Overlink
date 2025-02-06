// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "OBM_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class OVERLINK_API AOBM_PlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void UpdateRotation(float DeltaTime) override;

private:
	FVector LastFrameGravity = FVector::ZeroVector;
};

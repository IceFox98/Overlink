// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "OvrlAIController.generated.h"

/**
 * 
 */
UCLASS()
class OVERLINK_API AOvrlAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	 
	AOvrlAIController();
	
public:
	
	/** Update direction AI is looking based on FocalPoint */
	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn = true) override;
};

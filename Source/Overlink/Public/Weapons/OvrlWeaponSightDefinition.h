// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "OvrlWeaponSightDefinition.generated.h"

class UCameraModifier;

UENUM()
enum class ESightMagnification
{
	One UMETA(DisplayName = "1x"),
	OneHalf UMETA(DisplayName = "1.5x"),
	Two UMETA(DisplayName = "2x"),
	TwoHalf UMETA(DisplayName = "2.5x"),
	Three UMETA(DisplayName = "3x"),
	Four UMETA(DisplayName = "4x"),
	Five UMETA(DisplayName = "5x")
};

/**
 *
 */
UCLASS()
class OVERLINK_API UOvrlWeaponSightDefinition : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Ovrl Weapon Sight Definition")
	ESightMagnification SightMagnification;

	UPROPERTY(EditAnywhere, Category = "Ovrl Weapon Sight Definition")
	TSubclassOf<UCameraModifier> CameraFOV;

public:

	float GetMagnifiedFOV(float InFOV);
};

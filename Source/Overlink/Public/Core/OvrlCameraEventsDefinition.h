// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "OvrlCameraEventsDefinition.generated.h"

class UOvrlCameraModifierBase;

UENUM()
enum class ECameraFeedbackEvent : uint8
{
	StartRun,
	StopRun,
	Landed,
	SlideStart,
	SlideEnd,
	Hit
};

USTRUCT()
struct FCameraModifierEventData
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	TSubclassOf<UOvrlCameraModifierBase> ModifierClass;

	UPROPERTY(EditAnywhere)
	float BaseScale = 1.f;

	UPROPERTY(EditAnywhere)
	float Priority = 0.f;

	UPROPERTY(EditAnywhere)
	bool bLooping = false;

	UPROPERTY(EditAnywhere)
	bool bCanStack = false;
};

UCLASS()
class UOvrlCameraEventsDefinition : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Ovrl Camera Event Definition")
	TMap<ECameraFeedbackEvent, FCameraModifierEventData> EventMap;
};

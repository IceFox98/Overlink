// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTags.h"
#include "OvrlFoleyAudioBank.generated.h"

USTRUCT()
struct FSurfaceSounds
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Surface Sound")
	TMap<TEnumAsByte<EPhysicalSurface>, TObjectPtr<USoundBase>> SurfaceSounds;
};

/**
 *
 */
UCLASS()
class OVERLINK_API UOvrlFoleyAudioBank : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	USoundBase* GetSound(const FGameplayTag& Event, EPhysicalSurface SurfaceType) const;

public:

	UPROPERTY(EditAnywhere, Category = "Ovrl Foley Audio Bank", meta = (Categories = "Ovrl.FoleyEvent"))
	TMap<FGameplayTag, FSurfaceSounds> Assets;
};

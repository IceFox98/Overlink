// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "OvrlGameplayTags.h"
#include "OvrlAnimManagerData.generated.h"

class UOvrlAnimModifierBase;

/**
 *
 */
UCLASS()
class OVERLINK_API UOvrlAnimManagerData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	FGameplayTag StanceToCheck = OvrlStanceTags::Standing;

	UPROPERTY(EditAnywhere)
	FVector StartTranslation;

	UPROPERTY(EditAnywhere)
	FRotator StartRotation;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UOvrlAnimModifierBase>> ModifierClasses;
};

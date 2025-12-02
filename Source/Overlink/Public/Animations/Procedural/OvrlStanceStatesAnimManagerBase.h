// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OvrlGameplayTags.h"
#include "OvrlStanceStatesAnimManagerBase.generated.h"

class UOvrlStanceAnimComponentBase;
class AOvrlPlayerCharacter;

/**
 *
 */
UCLASS(EditInlineNew, DefaultToInstanced)
class OVERLINK_API UOvrlStanceStatesAnimManagerBase : public UObject
{
	GENERATED_BODY()

public:

	void Initialize(AOvrlPlayerCharacter* PlayerCharacter, FVector* OutTranslationPtr, FRotator* OutRotationPtr);

	void Update(float DeltaTime);

protected:

	UFUNCTION()
	void OnStanceChanged(const FGameplayTag& OldStance, const FGameplayTag& NewStance);

	UFUNCTION()
	void OnGaitChanged(const FGameplayTag& OldGait, const FGameplayTag& NewGait);

public:

	UPROPERTY(EditAnywhere)
	FVector StartTranslation;

	UPROPERTY(EditAnywhere)
	FRotator StartRotation;

protected:

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UOvrlStanceAnimComponentBase>> ComponentClasses;

	UPROPERTY()
	TArray<TObjectPtr<UOvrlStanceAnimComponentBase>> Components;

	FVector* OutTranslation;
	FRotator* OutRotation;

	FGameplayTag StanceToCheck = OvrlStanceTags::Standing;
};

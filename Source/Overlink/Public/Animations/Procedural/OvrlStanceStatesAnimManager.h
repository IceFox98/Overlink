// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OvrlGameplayTags.h"
#include "OvrlStanceStatesAnimManager.generated.h"

class UOvrlAnimModifierBase;
class AOvrlPlayerCharacter;

/**
 *
 */
UCLASS(EditInlineNew, DefaultToInstanced)
class OVERLINK_API UOvrlStanceStatesAnimManager : public UObject
{
	GENERATED_BODY()

public:

	void Initialize(AOvrlPlayerCharacter* PlayerCharacter);

	//void Update(float DeltaTime, FVector& OutStartTranslation, FRotator& OutStartRotation, FVector& OutTranslation, FRotator& OutRotation);

	FORCEINLINE bool IsActive() { return bShouldUpdateStartPosition; };

	FORCEINLINE TArray<UOvrlAnimModifierBase*> GetModifiers() const { return Modifiers; };

	void GetStartingPosition(float DeltaTime, FVector& OutTranslation, FRotator& OutRotation);

protected:

	UFUNCTION()
	void OnStanceChanged(const FGameplayTag& OldStance, const FGameplayTag& NewStance);

	UFUNCTION()
	void OnGaitChanged(const FGameplayTag& OldGait, const FGameplayTag& NewGait);

protected:

	UPROPERTY(EditAnywhere)
	FGameplayTag StanceToCheck = OvrlStanceTags::Standing;

	UPROPERTY(EditAnywhere)
	FVector StartTranslation;

	UPROPERTY(EditAnywhere)
	FRotator StartRotation;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UOvrlAnimModifierBase>> ModifierClasses;

private:

	UPROPERTY()
	TArray<TObjectPtr<UOvrlAnimModifierBase>> Modifiers;

	bool bShouldUpdateStartPosition = false;

	float Alpha = 1.f;
};

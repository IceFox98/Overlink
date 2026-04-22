// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OvrlGameplayTags.h"
#include "OvrlStanceStatesAnimManager.generated.h"

class UOvrlAnimModifierBase;
class UOvrlAnimManagerData;
class AOvrlCharacterBase;

/**
 *
 */
UCLASS(EditInlineNew, DefaultToInstanced)
class OVERLINK_API UOvrlStanceStatesAnimManager : public UObject
{
	GENERATED_BODY()

public:

	void Initialize(AOvrlCharacterBase* PlayerCharacter, const UOvrlAnimManagerData* ManagerData);

	FORCEINLINE bool IsActive() { return bShouldUpdateStartPosition; };
	FORCEINLINE TArray<UOvrlAnimModifierBase*> GetModifiers() const { return Modifiers; };

	void GetStartingPosition(float DeltaTime, FVector& OutTranslation, FRotator& OutRotation);

protected:

	UFUNCTION()
	void OnStanceChanged(const FGameplayTag& OldStance, const FGameplayTag& NewStance);

	UFUNCTION()
	void OnGaitChanged(const FGameplayTag& OldGait, const FGameplayTag& NewGait);

protected:

	FGameplayTag StanceToCheck = OvrlStanceTags::Standing;
	FVector StartTranslation;
	FRotator StartRotation;
	
	UPROPERTY()
	TArray<TObjectPtr<UOvrlAnimModifierBase>> Modifiers;

private:

	bool bShouldUpdateStartPosition = false;
	float Alpha = 1.f;
};

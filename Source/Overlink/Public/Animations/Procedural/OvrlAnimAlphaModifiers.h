// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OvrlGameplayTags.h"
#include "OvrlAnimAlphaModifiers.generated.h"

class UOvrlRangedWeaponAnimInstance;

UCLASS(NotBlueprintable, Abstract, EditInlineNew, DefaultToInstanced)
class OVERLINK_API UOvrlAnimAlphaModifierBase : public UObject
{
	GENERATED_BODY()

public:

	virtual void Initialize() {};
	virtual void ModifyAlpha(float& OutAlpha) {};
};

UCLASS()
class OVERLINK_API UOvrlWeaponAimingAnimAlphaModifier : public UOvrlAnimAlphaModifierBase
{
	GENERATED_BODY()

public:

	virtual void Initialize();
	virtual void ModifyAlpha(float& OutAlpha);

public:

	UPROPERTY(EditAnywhere)
	float AlphaMultiplier = 1.f;

protected:

	UPROPERTY()
	TWeakObjectPtr<UOvrlRangedWeaponAnimInstance> RangedWeaponAnimInstance;
};
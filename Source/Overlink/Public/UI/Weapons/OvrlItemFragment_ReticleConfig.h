// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inventory/OvrlItemDefinition.h"
#include "OvrlItemFragment_ReticleConfig.generated.h"

/**
 *
 */
UCLASS()
class OVERLINK_API UOvrlItemFragment_ReticleConfig : public UOvrlItemFragment
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Item Fragment")
		TSubclassOf<class UOvrlReticleWidgetBase> ReticleWidgetClass;
};

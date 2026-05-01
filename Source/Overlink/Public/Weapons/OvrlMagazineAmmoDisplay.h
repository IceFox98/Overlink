// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "OvrlMagazineAmmoDisplay.generated.h"

/**
 * 
 */
UCLASS()
class OVERLINK_API UOvrlMagazineAmmoDisplay : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	void UpdateMagazineAmmo(int32 AmmoCount);

protected:
	UMaterialInstanceDynamic* GetDisplayDynamicMat();

private:
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DisplayDynamicMat;
};

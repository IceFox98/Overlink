// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OvrlReticleWidgetBase.generated.h"

class UImage;
class AOvrlRangedWeaponInstance;

/**
 *
 */
UCLASS()
class OVERLINK_API UOvrlReticleWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Ovrl Reticle Widget Base")
		virtual void InitializeFromWeapon(AOvrlRangedWeaponInstance* Weapon);

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Reticle Widget Base", meta = (AllowPrivateAccess = true))
		TObjectPtr<AOvrlRangedWeaponInstance> WeaponInstance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Reticle Widget Baset", meta = (BindWidget))
		TObjectPtr<UImage> HitMarkerImage;
};

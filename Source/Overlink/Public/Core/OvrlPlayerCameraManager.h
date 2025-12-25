// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "Core/OvrlCameraEventsDefinition.h"
#include "OvrlPlayerCameraManager.generated.h"

class UOvrlCameraModifierBase;

/**
 * 
 */
UCLASS()
class OVERLINK_API AOvrlPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
public:

	static AOvrlPlayerCameraManager* Get(const UObject* WorldContextObject);

	template<typename T>
	T* GetOrAddCameraModifier(TSubclassOf<UCameraModifier> ModifierClass)
	{
		if (UCameraModifier* FoundModifier = FindCameraModifierByClass(ModifierClass))
		{
			return Cast<T>(FoundModifier);
		}
		else
		{
			return Cast<T>(AddNewCameraModifier(ModifierClass));
		}
	};

public:

	void HandleCameraEvent(ECameraFeedbackEvent Event, float Strength);

public:

	UPROPERTY(EditAnywhere, Category = "Ovrl Camera Event Definition")
	TObjectPtr<UOvrlCameraEventsDefinition> CameraEventsDefinition;

private:

	UPROPERTY()
	TObjectPtr<UOvrlCameraModifierBase> CameraModifier;

};

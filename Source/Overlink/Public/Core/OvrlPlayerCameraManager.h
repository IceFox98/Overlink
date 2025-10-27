// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "OvrlPlayerCameraManager.generated.h"

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

};

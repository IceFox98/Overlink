// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OvrlSaveableObject.generated.h"


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UOvrlSaveableObject : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OVERLINK_API IOvrlSaveableObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Ovrl Saveable Object")
	void OnPreLoad();
	
	UFUNCTION(BlueprintNativeEvent, Category = "Ovrl Saveable Object")
	void OnLoad();
	
	UFUNCTION(BlueprintNativeEvent, Category = "Ovrl Saveable Object")
	void OnPreSave();
	
	UFUNCTION(BlueprintNativeEvent, Category = "Ovrl Saveable Object")
	void OnSave();
};

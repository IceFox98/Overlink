// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/OvrlPlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"

AOvrlPlayerCameraManager* AOvrlPlayerCameraManager::Get(const UObject* WorldContextObject)
{
	return Cast<AOvrlPlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(WorldContextObject, 0));
}

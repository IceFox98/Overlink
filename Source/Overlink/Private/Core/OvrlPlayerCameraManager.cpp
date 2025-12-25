// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/OvrlPlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "Player/CameraModifiers/OvrlCameraModifierBase.h"

AOvrlPlayerCameraManager* AOvrlPlayerCameraManager::Get(const UObject* WorldContextObject)
{
	return Cast<AOvrlPlayerCameraManager>(UGameplayStatics::GetPlayerCameraManager(WorldContextObject, 0));
}

void AOvrlPlayerCameraManager::HandleCameraEvent(ECameraFeedbackEvent Event, float Strength)
{
	if (!ensure(CameraEventsDefinition))
	{
		return;
	}

	if (CameraModifier)
	{
		CameraModifier->DisableModifier();
	}

	const FCameraModifierEventData* Data = CameraEventsDefinition->EventMap.Find(Event);
	if (!Data || !Data->ModifierClass)
	{
		return;
	}

	CameraModifier = GetOrAddCameraModifier<UOvrlCameraModifierBase>(Data->ModifierClass);
	if (CameraModifier)
	{
		CameraModifier->EnableModifier();
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/OvrlGameConstants.h"

#include "Core/OvrlGameInstance.h"

const UOvrlGameConstants* UOvrlGameConstants::Get(const UObject* WorldContextObject)
{
	if (const UOvrlGameInstance* GameInstance = UOvrlGameInstance::Get(WorldContextObject))
	{
		return GameInstance->GetGameConstants();
	}
	return nullptr;
}

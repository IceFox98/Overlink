// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/OvrlGameInstance.h"

#include "SaveSystem/OvrlSaveGameSubsystem.h"
#include "UI/OvrlGameUIManagerSubsystem.h"

UOvrlGameInstance::UOvrlGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UOvrlGameInstance::Init()
{
	Super::Init();

	UOvrlSaveGameSubsystem* SG = GetSubsystem<UOvrlSaveGameSubsystem>();
}

UOvrlGameInstance* UOvrlGameInstance::Get(const UObject* WorldContextObject)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject))
	{
		return World->GetGameInstance<UOvrlGameInstance>();
	}
	return nullptr;
}

int32 UOvrlGameInstance::AddLocalPlayer(ULocalPlayer* NewPlayer, FPlatformUserId UserId)
{
	return Super::AddLocalPlayer(NewPlayer, UserId);
}

bool UOvrlGameInstance::RemoveLocalPlayer(ULocalPlayer* ExistingPlayer)
{
	GetSubsystem<UOvrlGameUIManagerSubsystem>()->NotifyPlayerDestroyed(ExistingPlayer);

	return Super::RemoveLocalPlayer(ExistingPlayer);
}

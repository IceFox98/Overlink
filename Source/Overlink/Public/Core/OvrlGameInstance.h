// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"

#include "OvrlGameInstance.generated.h"


UCLASS(Config = Game)
class OVERLINK_API UOvrlGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UOvrlGameInstance(const FObjectInitializer& ObjectInitializer);
	
public:

	virtual void Init() override;

	virtual int32 AddLocalPlayer(ULocalPlayer* NewPlayer, FPlatformUserId UserId) override;
	virtual bool RemoveLocalPlayer(ULocalPlayer* ExistingPlayer) override;
};

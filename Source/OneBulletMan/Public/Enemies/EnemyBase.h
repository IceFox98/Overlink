// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Pawn/OBM_PawnBase.h"
#include "EnemyBase.generated.h"


UCLASS()
class ONEBULLETMAN_API AEnemyBase : public AOBM_PawnBase
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemyBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

};

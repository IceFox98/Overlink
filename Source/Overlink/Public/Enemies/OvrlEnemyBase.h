// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Pawn/OBM_PawnBase.h"

#include "OBM_EnemyBase.generated.h"


UCLASS()
class OVERLINK_API AOBM_EnemyBase : public AOBM_PawnBase
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AOBM_EnemyBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

};

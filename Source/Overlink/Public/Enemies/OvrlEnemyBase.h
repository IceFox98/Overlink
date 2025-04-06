// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Pawn/OvrlPawnBase.h"
#include "Core//OvrlDamageable.h"

#include "OvrlEnemyBase.generated.h"


UCLASS()
class OVERLINK_API AOvrlEnemyBase : public AOvrlPawnBase, public IOvrlDamageable
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AOvrlEnemyBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

};

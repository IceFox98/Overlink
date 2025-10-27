// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Pawn/OvrlPawnBase.h"
#include "Core//OvrlDamageable.h"

#include "OvrlEnemyBase.generated.h"

USTRUCT(BlueprintType)
struct FOvrlKillMessage
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly)
		AActor* Killer;

	UPROPERTY(BlueprintReadOnly)
		AActor* Victim;
};

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

	virtual void HandleDeath(AActor* InInstigator) override;

};

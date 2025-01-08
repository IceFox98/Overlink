// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/EnemyBase.h"
#include "AbilitySystem/OBM_AbilitySet.h"
#include "AbilitySystem/OBM_AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/OBM_HealthSet.h"
#include "Player/Components/HealthComponent.h"
#include "UObject/UObjectBaseUtility.h"

#include "OBM_Utils.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


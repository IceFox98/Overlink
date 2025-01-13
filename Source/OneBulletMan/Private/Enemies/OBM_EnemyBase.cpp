// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/OBM_EnemyBase.h"

// Sets default values
AOBM_EnemyBase::AOBM_EnemyBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AOBM_EnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOBM_EnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


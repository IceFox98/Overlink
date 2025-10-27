// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/OvrlEnemyBase.h"
#include "OvrlGameplayTags.h"

#include "GameFramework/GameplayMessageSubsystem.h"

// Sets default values
AOvrlEnemyBase::AOvrlEnemyBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AOvrlEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOvrlEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AOvrlEnemyBase::HandleDeath(AActor* InInstigator)
{
	FOvrlKillMessage KillMessage;
	KillMessage.Killer = InInstigator;
	KillMessage.Victim = this;

	// Notify kill
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
	MessageSystem.BroadcastMessage(OvrlCoreTags::EnemyKilled, KillMessage);

	Super::HandleDeath(InInstigator);
}


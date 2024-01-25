// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/OBM_PawnBase.h"
#include "AbilitySystem/OBM_AbilitySet.h"
#include "AbilitySystem/OBM_AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/OBM_HealthSet.h"
#include "Player/Components/HealthComponent.h"
#include "UObject/UObjectBaseUtility.h"

#include "../OBM_Utils.h"

// Sets default values
AOBM_PawnBase::AOBM_PawnBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	AbilitySystemComponent = CreateDefaultSubobject<UOBM_AbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}

// Called when the game starts or when spawned
void AOBM_PawnBase::BeginPlay()
{
	Super::BeginPlay();
	
	HealthComponent->InitializeWithASC(AbilitySystemComponent);
	HealthComponent->OnOutOfHealth.AddDynamic(this, &AOBM_PawnBase::HandleDeath);

	for (const UOBM_AbilitySet* AbilitySet : AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr, this);
		}
	}
}

// Called every frame
void AOBM_PawnBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AOBM_PawnBase::HandleDeath(AActor* InInstigator)
{
	OBM_LOG_INFO(LogTemp, false, "%s is out of health, destroying. Killer: %s", *GetName(), *GetNameSafe(InInstigator));

	Destroy();
}


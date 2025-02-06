// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/OBM_CharacterBase.h"
#include "Player/Components/OBM_HealthComponent.h"
#include "AbilitySystem/OBM_AbilitySet.h"
#include "AbilitySystem/OBM_AbilitySystemComponent.h"

#include "OBM_Utils.h"

// Sets default values
AOBM_CharacterBase::AOBM_CharacterBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UOBM_HealthComponent>(TEXT("HealthComponent"));
	AbilitySystemComponent = CreateDefaultSubobject<UOBM_AbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	GripPointName = FName("GripPoint");
}

// Called when the game starts or when spawned
void AOBM_CharacterBase::BeginPlay()
{
	Super::BeginPlay();

	HealthComponent->InitializeWithASC(AbilitySystemComponent);
	HealthComponent->OnOutOfHealth.AddDynamic(this, &AOBM_CharacterBase::HandleDeath);

	for (const UOBM_AbilitySet* AbilitySet : AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr, this);
		}
	}
}

// Called every frame
void AOBM_CharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AOBM_CharacterBase::HandleDeath(AActor* InInstigator)
{
	OBM_LOG_INFO(LogTemp, false, "%s is out of health, destroying. Killer: %s", *GetName(), *GetNameSafe(InInstigator));

	Destroy();
}

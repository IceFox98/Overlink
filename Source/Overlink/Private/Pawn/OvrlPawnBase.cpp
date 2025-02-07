// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawn/OvrlPawnBase.h"
#include "AbilitySystem/OvrlAbilitySet.h"
#include "AbilitySystem/Attributes/OvrlHealthSet.h"
#include "AbilitySystem/OvrlAbilitySystemComponent.h"
#include "Player/Components/OvrlHealthComponent.h"

#include "UObject/UObjectBaseUtility.h"

#include "OvrlUtils.h"

// Sets default values
AOvrlPawnBase::AOvrlPawnBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UOvrlHealthComponent>(TEXT("HealthComponent"));
	AbilitySystemComponent = CreateDefaultSubobject<UOvrlAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}

// Called when the game starts or when spawned
void AOvrlPawnBase::BeginPlay()
{
	Super::BeginPlay();
	
	HealthComponent->InitializeWithASC(AbilitySystemComponent);
	HealthComponent->OnOutOfHealth.AddDynamic(this, &AOvrlPawnBase::HandleDeath);

	for (const UOvrlAbilitySet* AbilitySet : AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr, this);
		}
	}
}

// Called every frame
void AOvrlPawnBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

UAbilitySystemComponent* AOvrlPawnBase::GetAbilitySystemComponent() const
{
	return Cast<UAbilitySystemComponent>(AbilitySystemComponent);
}

void AOvrlPawnBase::HandleDeath(AActor* InInstigator)
{
	OVRL_LOG_INFO(LogTemp, false, "%s is out of health, destroying. Killer: %s", *GetName(), *GetNameSafe(InInstigator));

	Destroy();
}


// Fill out your copyright notice in the Description page of Project Settings.

#include "Pawn/OvrlPawnBase.h"
#include "AbilitySystem/OvrlAbilitySet.h"
#include "AbilitySystem/Attributes/OvrlHealthSet.h"
#include "AbilitySystem/OvrlAbilitySystemComponent.h"
#include "Player/Components/OvrlHealthComponent.h"

#include "UObject/UObjectBaseUtility.h"

#include "OvrlUtils.h"

AOvrlPawnBase::AOvrlPawnBase()
{
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UOvrlHealthComponent>(TEXT("HealthComponent"));
	AbilitySystemComponent = CreateDefaultSubobject<UOvrlAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
}

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

UAbilitySystemComponent* AOvrlPawnBase::GetAbilitySystemComponent() const
{
	return Cast<UAbilitySystemComponent>(AbilitySystemComponent);
}

void AOvrlPawnBase::HandleDeath(AActor* InInstigator)
{
	OVRL_LOG_INFO(LogTemp, false, "%s is out of health, destroying. Killer: %s", *GetName(), *GetNameSafe(InInstigator));

	Destroy();
}

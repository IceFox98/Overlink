// Fill out your copyright notice in the Description page of Project Settings.


	// Instanced, EditInline
		// TArray<UStanceStatesAnimManagerBase> Managers;


		// UStanceStatesAnimManagerBase abstract
		// - Ha una referenza al movement component
		// - Gli verrà passato il puntatore all'OutTranslation/OutRotation
		// - Si registra ai cambi di stance
		// - Si disattiva da solo l'array di component (in modo smooth) in base al cambio di stance
		// - Ha una funzione Start(), dove setta la posizione di default per la mano o altro
		// - Nella tick esegue la Update di ogni component, a prescindere dallo stato (idle, walk, ecc..)


		// UStandingStatesAnimManager
		// OnStanceChanged(OldStance, NewStance)
		// - Se la NewStance è quella del manager (Standing, hardcodata?)
		//		- Chiama la Enable() su tutti i component
		// - Altrimenti
		//		- Chiama la Disable() in tutti i componenti


		// UCrouchingStatesAnimManager
		// 




#include "Animations/Procedural/OvrlStanceStatesAnimManagerBase.h"

#include "Player/OvrlPlayerCharacter.h"
#include "Player/OvrlPlayerAnimInstance.h"
#include "Player/Components/OvrlCharacterMovementComponent.h"
#include "Animations/Procedural/OvrlStanceAnimComponentBase.h"

void UOvrlStanceStatesAnimManagerBase::Initialize(AOvrlPlayerCharacter* PlayerCharacter, FVector* OutTranslationPtr, FRotator* OutRotationPtr)
{
	OutTranslation = OutTranslationPtr;
	OutRotation = OutRotationPtr;

	*OutTranslation = StartTranslation;
	*OutRotation = StartRotation;

	UOvrlCharacterMovementComponent* CharacterMovementComponent = Cast<UOvrlCharacterMovementComponent>(PlayerCharacter->GetCharacterMovement());

	CharacterMovementComponent->OnStanceChanged.AddUniqueDynamic(this, &UOvrlStanceStatesAnimManagerBase::OnStanceChanged);
	CharacterMovementComponent->OnGaitChanged.AddUniqueDynamic(this, &UOvrlStanceStatesAnimManagerBase::OnGaitChanged);

	for (TSubclassOf<UOvrlStanceAnimComponentBase> ComponentClass : ComponentClasses)
	{
		UOvrlStanceAnimComponentBase* Component = NewObject<UOvrlStanceAnimComponentBase>(GetOuter(), ComponentClass);
		if (Component)
		{
			Component->Initialize(PlayerCharacter);
			Components.Add(Component);
		}
	}
}

void UOvrlStanceStatesAnimManagerBase::Update(float DeltaTime)
{
	for (UOvrlStanceAnimComponentBase* Component : Components)
	{
		if (Component)
		{
			Component->Update(DeltaTime, *OutTranslation, *OutRotation);
		}
	}
}

void UOvrlStanceStatesAnimManagerBase::OnStanceChanged(const FGameplayTag& OldStance, const FGameplayTag& NewStance)
{
	for (UOvrlStanceAnimComponentBase* Component : Components)
	{
		if (Component)
		{
			Component->Toggle(NewStance == StanceToCheck);
		}
	}
}

void UOvrlStanceStatesAnimManagerBase::OnGaitChanged(const FGameplayTag& OldGait, const FGameplayTag& NewGait)
{
	for (UOvrlStanceAnimComponentBase* Component : Components)
	{
		if (Component)
		{
			Component->CurrentGait = NewGait;
		}
	}
}

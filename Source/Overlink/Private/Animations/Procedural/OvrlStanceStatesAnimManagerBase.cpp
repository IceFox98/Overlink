// Fill out your copyright notice in the Description page of Project Settings.

#include "Animations/Procedural/OvrlStanceStatesAnimManagerBase.h"

#include "Player/OvrlPlayerCharacter.h"
#include "Player/OvrlPlayerAnimInstance.h"
#include "Player/Components/OvrlCharacterMovementComponent.h"
#include "Animations/Procedural/OvrlStanceAnimComponentBase.h"

void UOvrlStanceStatesAnimManagerBase::Initialize(AOvrlPlayerCharacter* PlayerCharacter, FVector* OutTranslationPtr, FRotator* OutRotationPtr)
{
	OutTranslation = OutTranslationPtr;
	OutRotation = OutRotationPtr;

	// Bind Stance and Gait delegates
	UOvrlCharacterMovementComponent* CharacterMovementComponent = Cast<UOvrlCharacterMovementComponent>(PlayerCharacter->GetCharacterMovement());
	CharacterMovementComponent->OnStanceChanged.AddUniqueDynamic(this, &UOvrlStanceStatesAnimManagerBase::OnStanceChanged);
	CharacterMovementComponent->OnGaitChanged.AddUniqueDynamic(this, &UOvrlStanceStatesAnimManagerBase::OnGaitChanged);

	// Initialize modifiers
	for (TSubclassOf<UOvrlAnimModifierBase> ComponentClass : ComponentClasses)
	{
		// Outer is Equipment Anim Instance
		UOvrlAnimModifierBase* Component = NewObject<UOvrlAnimModifierBase>(GetOuter(), ComponentClass);
		if (Component)
		{
			Component->Initialize(PlayerCharacter);
			Components.Add(Component);
		}
	}
}

void UOvrlStanceStatesAnimManagerBase::Update(float DeltaTime)
{
	for (UOvrlAnimModifierBase* Component : Components)
	{
		if (Component)
		{
			Component->Update(DeltaTime, *OutTranslation, *OutRotation);
		}
	}
}

void UOvrlStanceStatesAnimManagerBase::OnStanceChanged(const FGameplayTag& OldStance, const FGameplayTag& NewStance)
{
	for (UOvrlAnimModifierBase* Component : Components)
	{
		if (Component)
		{
			Component->Toggle(NewStance == StanceToCheck);
		}
	}
}

void UOvrlStanceStatesAnimManagerBase::OnGaitChanged(const FGameplayTag& OldGait, const FGameplayTag& NewGait)
{
	for (UOvrlAnimModifierBase* Component : Components)
	{
		if (Component)
		{
			Component->SetTag(NewGait);
		}
	}
}

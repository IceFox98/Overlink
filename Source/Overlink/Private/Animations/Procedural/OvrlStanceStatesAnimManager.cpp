// Fill out your copyright notice in the Description page of Project Settings.

#include "Animations/Procedural/OvrlStanceStatesAnimManager.h"

#include "Player/OvrlPlayerCharacter.h"
#include "Player/OvrlPlayerAnimInstance.h"
#include "Player/Components/OvrlCharacterMovementComponent.h"
#include "Animations/Procedural/OvrlAnimModifiers.h"

void UOvrlStanceStatesAnimManager::Initialize(AOvrlPlayerCharacter* PlayerCharacter)
{
	// Bind Stance and Gait delegates
	UOvrlCharacterMovementComponent* CharacterMovementComponent = Cast<UOvrlCharacterMovementComponent>(PlayerCharacter->GetCharacterMovement());
	CharacterMovementComponent->OnStanceChanged.AddUniqueDynamic(this, &UOvrlStanceStatesAnimManager::OnStanceChanged);
	CharacterMovementComponent->OnGaitChanged.AddUniqueDynamic(this, &UOvrlStanceStatesAnimManager::OnGaitChanged);

	// Initialize modifiers
	for (TSubclassOf<UOvrlAnimModifierBase> ModifierClass : ModifierClasses)
	{
		// Outer is Equipment Anim Instance
		UOvrlAnimModifierBase* Modifier = NewObject<UOvrlAnimModifierBase>(GetOuter(), ModifierClass);
		if (Modifier)
		{
			Modifier->Initialize(PlayerCharacter);
			Modifiers.Add(Modifier);
		}
	}
}

void UOvrlStanceStatesAnimManager::Update(float DeltaTime, FVector& OutStartTranslation, FRotator& OutStartRotation, FVector& OutTranslation, FRotator& OutRotation)
{
	const float TargetAlpha = bShouldUpdateStartPosition ? 1.f : 0.f;
	Alpha = FMath::FInterpTo(Alpha, TargetAlpha, DeltaTime, 20.f);

	OutStartTranslation += StartTranslation * Alpha;
	OutStartRotation += StartRotation * Alpha;

	for (UOvrlAnimModifierBase* Modifier : Modifiers)
	{
		if (Modifier)
		{
			Modifier->Update(DeltaTime, OutTranslation, OutRotation);
		}
	}
}

void UOvrlStanceStatesAnimManager::OnStanceChanged(const FGameplayTag& OldStance, const FGameplayTag& NewStance)
{
	bShouldUpdateStartPosition = NewStance == StanceToCheck;

	for (UOvrlAnimModifierBase* Modifier : Modifiers)
	{
		if (Modifier)
		{
			Modifier->Toggle(NewStance == StanceToCheck);
		}
	}
}

void UOvrlStanceStatesAnimManager::OnGaitChanged(const FGameplayTag& OldGait, const FGameplayTag& NewGait)
{
	for (UOvrlAnimModifierBase* Modifier : Modifiers)
	{
		if (Modifier)
		{
			Modifier->SetTag(NewGait);
		}
	}
}

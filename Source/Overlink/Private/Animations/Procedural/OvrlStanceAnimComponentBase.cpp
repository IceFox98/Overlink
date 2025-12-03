// Fill out your copyright notice in the Description page of Project Settings.




		// UStanceAnimComponentBase abstract
		// - virtual Enable()/Disable() - Attiva/Disattiva una variabile bool, che impedisce l'aggiornamento dell'alpha controllata in Update()
		// - virtual Update() - Di base servirà a controllare l'alpha del component ed eseguire i calcoli per la translation/rotation.
		// Esegue in loop la CheckStart()
		// Viene eseguita solo quando la curva iniziale ha finito (e se è partita)
		// - virtual CheckStart() Controlla se deve o no eseguire la curva iniziale (nel component di Idle, ci potrebbe essere la curva di "stop" del walk/run).
		// Se l'alpha passa da 0 ad un altro valore, fa partire la curva iniziale.

		// ?? Creare due classi base UStandingAnimComponent e UCrouchingAnimComponent da usare come classi per l'array nel manager ??

		// UStandingIdleAnimComponent
		// - Update(): Controlla se il Gait == Idle && bEnabled. In caso esegue le curve

		// UStandingMoveAnimComponent
		// - Update(): Controlla se il Gait == Walk && bEnabled. In caso esegue le curve

		// UMoveAnimComponent
		// - Ha una variabile FGameplayTag "GaitToCheck" che viene usata nell'update
		// - Update(): Controlla se il CurrentGait == GaitToCheck && bEnabled. In caso esegue le curve



#include "Animations/Procedural/OvrlStanceAnimComponentBase.h"
#include "Animations/OvrlRangedWeaponAnimInstance.h"

#include "Player/OvrlPlayerCharacter.h"
#include "Player/Components/OvrlCharacterMovementComponent.h"
#include "Weapons/OvrlRangedWeaponInstance.h"

#include "OvrlUtils.h"
#include "Overlink.h"

// Engine
#include "Curves/CurveVector.h"
#include "Kismet/KismetMathLibrary.h"

void UOvrlStanceAnimComponentBase::Initialize(AOvrlPlayerCharacter* InPlayerCharacter)
{
	PlayerCharacter = InPlayerCharacter;

	CharacterMovementComponent = Cast<UOvrlCharacterMovementComponent>(PlayerCharacter->GetCharacterMovement());
}

void UOvrlStanceAnimComponentBase::Update(float DeltaTime, FVector& OutTranslation, FRotator& OutRotation)
{
	const FRotator ComposedRotator = UKismetMathLibrary::ComposeRotators(PlayerCharacter->GetControlRotation(), FRotator(180.f, 0.f, 0.f));
	SpineRotation = FRotator(0.f, 0.f, ComposedRotator.Pitch);
}

void UOvrlStanceAnimComponentBase::Toggle(bool bEnable)
{
	bShouldUpdateAlpha = bEnable;
}

void UOvrlStanceAnimComponentBase::ComputeAlpha(float DeltaTime)
{
	if (CurrentGait == GaitToCheck && bShouldUpdateAlpha)
	{
		Alpha = 1.f; // We want fully alpha to be applied
	}
	else if (Alpha <= KINDA_SMALL_NUMBER)
	{
		Alpha = 0.f;
	}
	else
	{
		// Smoothly decrease alpha, to avoid jerky movements
		Alpha = FMath::FInterpTo(Alpha, 0.f, DeltaTime, RecoverySpeed);
	}
}

void UOvrlMoveAnimComponent::Update(float DeltaTime, FVector& OutTranslation, FRotator& OutRotation)
{
	ComputeAlpha(DeltaTime);

	if (Alpha <= 0.f)
	{
		return; // Just ignore calculations if Alpha is almost 0
	}

	FVector TargetWalkSwayTranslation = FVector::ZeroVector;
	FRotator TargetWalkSwayRotation = FRotator::ZeroRotator;

	const FVector LastInputVector = CharacterMovementComponent->GetLastInputVector();

	// Read the value only when player moves
	if (LastInputVector.Length() > 0.f)
	{
		const float ForwardAmount = FVector::DotProduct(LastInputVector, PlayerCharacter->GetActorForwardVector());
		const float RightwardAmount = FVector::DotProduct(LastInputVector, PlayerCharacter->GetActorRightVector());

		for (FCurveData& CurveData : CurvesData)
		{
			if (!CurveData.SwayCurve)
			{
				continue;
			}

			TargetWalkSwayTranslation += CurveData.SwayCurve->TranslationCurve->GetVectorValue(CurveData.Time) * CurveData.SwayCurve->TranslationMultiplier * FVector(1.f, ForwardAmount, 1.f);

			const FVector RotationCurve = CurveData.SwayCurve->RotationCurve->GetVectorValue(CurveData.Time) * CurveData.SwayCurve->RotationMultiplier;
			TargetWalkSwayRotation += FRotator(RotationCurve.Y, RotationCurve.Z, RotationCurve.X);

			CurveData.Time += DeltaTime * CurveData.SwayCurve->Frequency;
		}
	}
	else
	{
		for (FCurveData& CurveData : CurvesData)
		{
			CurveData.Time = 0.f;

			//if (CurveData.SwayCurve)
			//{
			//	CurveData.Time = Alpha * CurveData.SwayCurve->Frequency;
			//	if (CurveData.Time < 0.1f)
			//	{
			//		CurveData.Time = 0.f;
			//	}
			//}
		}
	}

	LastWalkSwayTranslation = FMath::VInterpTo(LastWalkSwayTranslation, TargetWalkSwayTranslation, DeltaTime, SwaySpeed) * Alpha;

	// Since its in component space, we have to rotate the vector in order to follow the player aim
	// I do this just for the anim BP, that's why I use 2 different variables.
	OutTranslation = SpineRotation.RotateVector(LastWalkSwayTranslation);

	OutRotation = FMath::RInterpTo(OutRotation, TargetWalkSwayRotation, DeltaTime, SwaySpeed) * Alpha;
}

void UOvrlRangedWeaponMoveAnimComponent::Initialize(AOvrlPlayerCharacter* InPlayerCharacter)
{
	Super::Initialize(InPlayerCharacter);

	RangedWeaponAnimInstance = Cast<UOvrlRangedWeaponAnimInstance>(GetOuter());
}

void UOvrlRangedWeaponMoveAnimComponent::ComputeAlpha(float DeltaTime)
{
	Super::ComputeAlpha(DeltaTime);

	if (RangedWeaponAnimInstance.IsValid())
	{
		if (AOvrlRangedWeaponInstance* RangedWeapon = RangedWeaponAnimInstance->GetEquippedWeapon())
		{
			if (RangedWeapon->IsADS())
			{
				Alpha *= RangedWeaponAnimInstance->GetWalkSwayAlphaADS();
			}
		}
	}
}

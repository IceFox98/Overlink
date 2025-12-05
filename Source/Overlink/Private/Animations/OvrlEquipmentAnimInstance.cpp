// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/OvrlEquipmentAnimInstance.h"

// Internal
#include "Player/OvrlPlayerCharacter.h"	
#include "Player/Components/OvrlCharacterMovementComponent.h"
#include "Inventory/OvrlInventoryComponent.h"
#include "Weapons/OvrlRangedWeaponInstance.h"
#include "Player/Components/OvrlCameraComponent.h"
#include "Player/OvrlPlayerAnimInstance.h"
#include "OvrlUtils.h"
#include "Overlink.h"
#include "Animations/Procedural/OvrlStanceStatesAnimManagerBase.h"

// Engine
#include "Curves/CurveVector.h"

UOvrlEquipmentAnimInstance::UOvrlEquipmentAnimInstance()
{
	MovementSwaySpeed = 20.f;
	MovementSwayMultiplier = 1.f;
	MovementSwayRollMultiplier = 2.f;

	WalkSwayTranslationMultiplier = FVector::One();
	WalkSwayRotationMultiplier = FVector::One();
	WalkSwaySpeed = 20.f;
	WalkSwayFrequency = 3.f;
	WalkSwayAlpha = 1.f;

	LookingSwayAlpha = 1.f;
	MovementSwayAlpha = 1.f;
	JumpSwayAlpha = 1.f;

	LookingSwayRotationLimit = FVector2D::One();
	LookingSwayMovementMultiplier = FVector::One();
	LookingSwayStiffness = .5f;
	LookingSwayCriticalDampingFactor = .8f;
}

void UOvrlEquipmentAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	CharacterMovementComponent = Cast<UOvrlCharacterMovementComponent>(PlayerCharacter->GetCharacterMovement());
}

void UOvrlEquipmentAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	check(PlayerCharacter);
	PlayerCharacter->GetInventoryComponent()->OnItemEquipped.AddDynamic(this, &UOvrlEquipmentAnimInstance::OnNewItemEquipped);

	LastPlayerCameraRotation = PlayerCharacter->GetCameraComponent()->GetComponentRotation();

	ensure(WalkSwayTranslationCurve);
	ensure(WalkSwayRotationCurve);
	ensure(JumpSwayCurve);

	for (UOvrlStanceStatesAnimManagerBase* Manager : Managers)
	{
		if (Manager)
		{
			Manager->Initialize(PlayerCharacter, &OutMoveTranslation, &OutMoveRotation);
		}
	}
}

void UOvrlEquipmentAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (GetParent())
	{
		SpineRotation = GetParent()->GetSpineRotation();
	}

	// Reset every frame
	OutMoveTranslation = FVector::ZeroVector;
	OutMoveRotation = FRotator::ZeroRotator;

	for (UOvrlStanceStatesAnimManagerBase* Manager : Managers)
	{
		if (Manager)
		{
			Manager->Update(DeltaTime);
		}
	}

	// Since we're in Component space, I have to rotate the vector in order to follow the player aim.
	OutMoveTranslation = SpineRotation.RotateVector(OutMoveTranslation);
}

void UOvrlEquipmentAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaTime)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaTime);

	if (IsValid(PlayerCharacter))
	{
		UpdateLookingSway(DeltaTime);
		UpdateMovementSway(DeltaTime);
		UpdateWalkSway(DeltaTime);
		UpdateJumpSway(DeltaTime);
		UpdateLeftHandIKAplha(DeltaTime);
		UpdateCrouchLeanAlpha(DeltaTime);
		UpdateRunPositionAlpha(DeltaTime);

		const FRotator CameraRotation = PlayerCharacter->GetCameraComponent()->GetComponentRotation();
		WallrunCameraTiltRotation = FRotator(CameraRotation.Roll, 0.f, 0.f);


		// for(comp : components)
		//	if(comp.IsEnabled())
		//		comp.UpdateValues(&OutTranslation, &OutRotation);


		// OnMovementModeChanged(OldMode, NewMode)
		//		components[OldMode].Disable()
		//		component[NewMode].Enable()


		// Component class
		// Enable()
		//	- Set








		// All'inizio
		// OutTranslation = 0
		// OutRotation = 0


		// TMap<StanceTag, FMovementModes>
		// struct FMovementModes
		// - MovementMode
		// - TArray<Component>


		// <Standing, 
		//	struct
		//		Walk, { Components }
		//		Run, { Components }
		//		Idle { Components }
		// ,
		// Crouch
		// struct
		//		Walk, { Components }
		//		Idle, { Components }
		// >


		// for(comp : components)
		//		comp.UpdateValues(&OutTranslation, &OutRotation);


		// Component class
		// CheckStart()
		//	- Controlla se deve o no eseguire la curva iniziale (nel component di Idle, ci potrebbe essere la curva di "stop" del walk/run)
		// UpdateValues() - Viene eseguita solo quando la curva iniziale ha finito











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

		// Questa anim instance ciclerà tutti i Manager a prescindere
	}
}

void UOvrlEquipmentAnimInstance::UpdateLookingSway(float DeltaTime)
{
	// Get camera delta movement
	const FRotator DeltaSwayRotation = UKismetMathLibrary::NormalizedDeltaRotator(LastPlayerCameraRotation, PlayerCharacter->GetCameraComponent()->GetComponentRotation());

	const float SwayPitch = FMath::Clamp(DeltaSwayRotation.Pitch, -LookingSwayRotationLimit.Y, LookingSwayRotationLimit.Y);
	const float SwayYaw = FMath::Clamp(-DeltaSwayRotation.Yaw, -LookingSwayRotationLimit.X, LookingSwayRotationLimit.X);
	const FRotator TargetSwayRotation = FRotator(SwayPitch, SwayYaw, 0.f);

	//LastLookingSwayRotation = UKismetMathLibrary::QuaternionSpringInterp(FQuat(LastLookingSwayRotation), FQuat(TargetSwayRotation), SpringStateRotation, LookingSwayStiffness, LookingSwayCriticalDampingFactor, DeltaTime, .0006f, .3f).Rotator();

	// Speed = 3.5
	LastLookingSwayRotation = UKismetMathLibrary::RInterpTo(LastLookingSwayRotation, TargetSwayRotation, DeltaTime, 3.5f);

	// Apply weapon sway looking to Anim BP
	LookingSwayTranslation = FVector(
		LastLookingSwayRotation.Yaw * LookingSwayMovementMultiplier.X,
		LastLookingSwayRotation.Yaw * LookingSwayMovementMultiplier.Z,
		LastLookingSwayRotation.Pitch * LookingSwayMovementMultiplier.Y
	);

	LookingSwayRotation = FRotator(
		LastLookingSwayRotation.Pitch * LookingSwayRotationMultiplier.Y,
		LastLookingSwayRotation.Yaw * LookingSwayRotationMultiplier.Z,
		LastLookingSwayRotation.Yaw * LookingSwayRotationMultiplier.X
	);




	//const FVector CameraDelta = FVector(SwayYaw, SwayPitch, 0.f);

	//LastLookingSwayTranslation = UKismetMathLibrary::VectorSpringInterp(LastLookingSwayTranslation, CameraDelta, SprintStateLookingSway, LookingSwayStiffness, LookingSwayCriticalDampingFactor, DeltaTime, 1.f, 0.3f);

	//// Apply weapon sway looking to Anim BP
	//LookingSwayTranslation = FVector(
	//	LastLookingSwayTranslation.X * LookingSwayMovementMultiplier.X,
	//	LastLookingSwayTranslation.X * LookingSwayMovementMultiplier.Z,
	//	LastLookingSwayTranslation.Y * LookingSwayMovementMultiplier.Y
	//);

	//LookingSwayRotation = FRotator(
	//	LastLookingSwayTranslation.Y * LookingSwayRotationMultiplier.Y,
	//	LastLookingSwayTranslation.X * LookingSwayRotationMultiplier.Z,
	//	LastLookingSwayTranslation.X * LookingSwayRotationMultiplier.X
	//);

	// Save the last sway rotation
	LastPlayerCameraRotation = PlayerCharacter->GetCameraComponent()->GetComponentRotation();
}

void UOvrlEquipmentAnimInstance::UpdateMovementSway(float DeltaTime)
{
	const FVector PlayerVelocity = CharacterMovementComponent->GetLastUpdateVelocity();

	// Calculate "how much" the player is moving in any directions
	// If it's moving fully forward, the Dot result will be equals to the MaxWalkSpeed, so the division will return 1.
	const float ForwardAmount = FVector::DotProduct(PlayerVelocity, PlayerCharacter->GetActorForwardVector()) / CharacterMovementComponent->MaxWalkSpeed;
	const float RightwardAmount = FVector::DotProduct(PlayerVelocity, PlayerCharacter->GetActorRightVector()) / CharacterMovementComponent->MaxWalkSpeed;

	const FVector TargetMovementAmount = FVector(RightwardAmount, -ForwardAmount, 0.f) * MovementSwayMultiplier;

	LastMovementSwayTranslation = FMath::VInterpTo(LastMovementSwayTranslation, TargetMovementAmount, DeltaTime, MovementSwaySpeed);

	MovementSwayRotation = FRotator(FMath::Clamp(LastMovementSwayTranslation.X, -1.f, 1.f) * -MovementSwayRollMultiplier, 0.f, 0.f);

	// Since its in component space, I have to rotate the vector in order to follow the player aim.
	// I do this just for the anim BP, that's why I use 2 different variables.
	MovementSwayTranslation = SpineRotation.RotateVector(LastMovementSwayTranslation);
}

void UOvrlEquipmentAnimInstance::UpdateWalkSway(float DeltaTime)
{
	if (!WalkSwayTranslationCurve)
	{
		OVRL_LOG_ERR(LogOverlink, false, "WalkSwayTraslationCurve is NULL!");
		return;
	}

	if (!WalkSwayRotationCurve)
	{
		OVRL_LOG_ERR(LogOverlink, false, "WalkSwayRotationCurve is NULL!");
		return;
	}

	FVector TargetWalkSwayTranslation = FVector::ZeroVector;
	FRotator TargetWalkSwayRotation = FRotator::ZeroRotator;

	const FVector LastInputVector = CharacterMovementComponent->GetLastInputVector();

	// Read the value only when player moves
	if (LastInputVector.Length() > 0.f)
	{
		const float ForwardAmount = FVector::DotProduct(LastInputVector, PlayerCharacter->GetActorForwardVector());
		const float RightwardAmount = FVector::DotProduct(LastInputVector, PlayerCharacter->GetActorRightVector());

		TargetWalkSwayTranslation = WalkSwayTranslationCurve->GetVectorValue(WalkSwayTime) * WalkSwayTranslationMultiplier * FVector(1.f, ForwardAmount, 1.f);

		const FVector RotationCurve = WalkSwayRotationCurve->GetVectorValue(WalkSwayTime) * WalkSwayRotationMultiplier;
		TargetWalkSwayRotation = FRotator(RotationCurve.Y, RotationCurve.Z, RotationCurve.X);

		WalkSwayTime += DeltaTime * WalkSwayFrequency;
	}
	else
	{
		WalkSwayTime = 0.f;
	}

	LastWalkSwayTranslation = FMath::VInterpTo(LastWalkSwayTranslation, TargetWalkSwayTranslation, DeltaTime, WalkSwaySpeed);

	// Since its in component space, we have to rotate the vector in order to follow the player aim
	// I do this just for the anim BP, that's why I use 2 different variables.
	WalkSwayTranslation = SpineRotation.RotateVector(LastWalkSwayTranslation);

	WalkSwayRotation = FMath::RInterpTo(WalkSwayRotation, TargetWalkSwayRotation, DeltaTime, WalkSwaySpeed);
}

void UOvrlEquipmentAnimInstance::UpdateJumpSway(float DeltaTime)
{
	if (!JumpSwayCurve)
	{
		OVRL_LOG_ERR(LogOverlink, false, "JumpSwayCurve is NULL!");
		return;
	}

	const FVector PlayerVelocity = CharacterMovementComponent->GetLastUpdateVelocity();

	// Calculate "how much" the player is jumping/falling
	// If the jump has just started, the Dot result will be equals to the JumpZVelocity, so the division will return 1.
	const float UpwardAmount = FVector::DotProduct(PlayerVelocity, PlayerCharacter->GetActorUpVector()) / CharacterMovementComponent->JumpZVelocity;
	const float RightwardAmount = FVector::DotProduct(PlayerVelocity, PlayerCharacter->GetActorRightVector()) / CharacterMovementComponent->MaxWalkSpeed;

	const float SideSwayMultiplier = .4f;

	// Apply some side sway movement, only when player jumps sideway
	const FVector TargetJumpAmount = JumpSwayCurve->GetVectorValue(-UpwardAmount) * FVector(1.f, -RightwardAmount * SideSwayMultiplier, 1.f);

	JumpSwayTranslation = UKismetMathLibrary::VectorSpringInterp(JumpSwayTranslation, TargetJumpAmount, SpringStateJump, .5f, .45f, DeltaTime, 0.005f, 5.f);

	// Calculate the rotation to apply when player jumps sideway
	const FVector JumpRotationVector = FVector(JumpSwayTranslation.Y, 0.f, JumpSwayTranslation.Y) * -JumpSwayRotationMultiplier;
	JumpSwayRotation = FRotator(0.f, JumpRotationVector.Z, JumpRotationVector.X);
}

void UOvrlEquipmentAnimInstance::UpdateCrouchLeanAlpha(float DeltaTime)
{
	const bool bShouldLean = CheckCrouchLeaning(); // Call virtual function
	const float TargetCrouchLeanAlpha = bShouldLean ? 1.f : 0.f;
	const float CrouchLeanSpeed = CalculateCrouchLeanSpeed();
	CrouchLeanAlpha = FMath::FInterpTo(CrouchLeanAlpha, TargetCrouchLeanAlpha, DeltaTime, CrouchLeanSpeed);
}

void UOvrlEquipmentAnimInstance::UpdateRunPositionAlpha(float DeltaTime)
{
	const bool bIsPlayerRunning = CharacterMovementComponent->IsRunning();
	const float TargetRunPositionAlpha = bIsPlayerRunning ? 0.f : 1.f;
	RunPositionAlpha = FMath::FInterpTo(RunPositionAlpha, TargetRunPositionAlpha, DeltaTime, 15.f);
}

void UOvrlEquipmentAnimInstance::UpdateLeftHandIKAplha(float DeltaTime)
{
	const bool bIsPlayerWallrunning = CharacterMovementComponent->IsWallrunning();
	const float TargetLeftHandIKAlpha = bIsPlayerWallrunning ? 0.f : 1.f;
	LeftHandIKAlpha = FMath::FInterpTo(LeftHandIKAlpha, TargetLeftHandIKAlpha, DeltaTime, 15.f);
}

void UOvrlEquipmentAnimInstance::OnNewItemEquipped(AOvrlEquipmentInstance* NewEquippedItem)
{
	EquippedItem = NewEquippedItem;
}

bool UOvrlEquipmentAnimInstance::CheckCrouchLeaning()
{
	return CharacterMovementComponent->IsCrouching();
}

float UOvrlEquipmentAnimInstance::CalculateCrouchLeanSpeed()
{
	return 10.f;
}

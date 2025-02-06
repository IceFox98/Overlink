// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/OBM_PlayerCharacter.h"

#include "Player/Components/OBM_CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/Components/OBM_HealthComponent.h"
#include "Player/Components/OBM_InteractionComponent.h"
#include "Player/Components/OBM_ParkourComponent.h"
#include "Player/Input/OBM_InputComponent.h"
#include "Player/Input/OBM_InputConfig.h"
#include "Inventory/OBM_InventoryComponent.h"
#include "AbilitySystem/OBM_AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/OBM_HealthSet.h"
#include "Player/Components/OBM_CharacterMovementComponent.h"

#include "Components/StaticMeshComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayEffectTypes.h"

#include "OBM_Utils.h"

AOBM_PlayerCharacter::AOBM_PlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UOBM_CharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	// Create a follow camera
	CameraComp = CreateDefaultSubobject<UOBM_CameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(RootComponent);
	CameraComp->bUsePawnControlRotation = true; // Camera does not rotate relative to arm
	CameraComp->SetupAttachment(GetMesh(), TEXT("head"));

	InteractionComponent = CreateDefaultSubobject<UOBM_InteractionComponent>(TEXT("InteractionComponent"));
	InventoryComponent = CreateDefaultSubobject<UOBM_InventoryComponent>(TEXT("InventoryComponent"));
	ParkourComponent = CreateDefaultSubobject<UOBM_ParkourComponent>(TEXT("ParkourComponent"));

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanJump = true;

	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;

	ThrowForce = 1200.f;
}

void AOBM_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	ParkourComponent->Initialize(this);

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AOBM_PlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	UOBM_InputComponent* OBM_IC = Cast<UOBM_InputComponent>(PlayerInputComponent);

	if (ensureMsgf(OBM_IC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UOBM_InputComponent or a subclass of it.")))
	{
		// This is where we actually bind and input action to a gameplay tag, which means that Gameplay Ability Blueprints will
		// be triggered directly by these input actions Triggered events. 
		TArray<uint32> BindHandles;
		OBM_IC->BindAbilityActions(InputConfig, this, &ThisClass::OnAbilityInputPressed, &ThisClass::OnAbilityInputReleased, /*out*/ BindHandles);

		OBM_IC->BindNativeAction(InputConfig, OBM_InputTags::Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
		OBM_IC->BindNativeAction(InputConfig, OBM_InputTags::Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
	}
}

void AOBM_PlayerCharacter::OnAbilityInputPressed(FGameplayTag InputTag)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityInputTagPressed(InputTag);
	}
}

void AOBM_PlayerCharacter::OnAbilityInputReleased(FGameplayTag InputTag)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityInputTagReleased(InputTag);
	}
}

void AOBM_PlayerCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

	if (Value.X != 0.0f)
	{
		if (true)
		{
			MovementRotation = UOBM_Utils::GetGravityRelativeRotation(Controller->GetControlRotation(), GetCharacterMovement()->GetGravityDirection());
			MovementRotation.Pitch = 0.f;
			MovementRotation = UOBM_Utils::GetGravityWorldRotation(MovementRotation, GetCharacterMovement()->GetGravityDirection());
		}

		const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
		AddMovementInput(MovementDirection, Value.X);
	}

	if (Value.Y != 0.0f)
	{
		if (true)
		{
			MovementRotation = UOBM_Utils::GetGravityRelativeRotation(Controller->GetControlRotation(), GetCharacterMovement()->GetGravityDirection());
			MovementRotation.Roll = 0.f;
			MovementRotation.Pitch = 0.f;

			MovementRotation = UOBM_Utils::GetGravityWorldRotation(MovementRotation, GetCharacterMovement()->GetGravityDirection());
		}

		const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
		AddMovementInput(MovementDirection, Value.Y);
	}
}

void AOBM_PlayerCharacter::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (Value.X != 0.0f)
	{
		AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		AddControllerPitchInput(Value.Y);
	}
}

void AOBM_PlayerCharacter::Crouch(bool bClientSimulation)
{
	Super::Crouch();

	SetStance(OBM_StanceTags::Crouching);
}

void AOBM_PlayerCharacter::UnCrouch(bool bClientSimulation)
{
	Super::UnCrouch();

	SetStance(OBM_StanceTags::Standing);
}

void AOBM_PlayerCharacter::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	// Use the character movement mode to set the locomotion mode to the right value. This allows you to have a
	// custom set of movement modes but still use the functionality of the default character movement component.

	switch (GetCharacterMovement()->MovementMode)
	{
	case MOVE_Walking:
	case MOVE_NavWalking:
		SetLocomotionMode(OBM_LocomotionModeTags::Grounded);
		break;

	case MOVE_Falling:
		SetLocomotionMode(OBM_LocomotionModeTags::InAir);
		break;

	default:
		SetLocomotionMode(FGameplayTag::EmptyTag);
		break;
	}

	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}

void AOBM_PlayerCharacter::SetLocomotionMode(const FGameplayTag& NewLocomotionMode)
{
	if (LocomotionMode == NewLocomotionMode)
		return;

	LocomotionMode = NewLocomotionMode;
}

void AOBM_PlayerCharacter::SetStance(const FGameplayTag& NewStance)
{
	if (Stance == NewStance)
		return;

	Stance = NewStance;
}

void AOBM_PlayerCharacter::SetGait(const FGameplayTag& NewGait)
{
	if (Gait == NewGait)
		return;

	Gait = NewGait;
}

void AOBM_PlayerCharacter::SetOverlayMode(const FGameplayTag& NewOverlayMode)
{
	if (OverlayMode == NewOverlayMode)
		return;

	OverlayMode = NewOverlayMode;
}

void AOBM_PlayerCharacter::Interact()
{
	//if (InteractionComponent)
	//{
	//	FInteractionData ObjData = InteractionComponent->FindInteractiveObject();

	//	if (ObjData.IsInteractive)
	//	{
	//		IInteractable* InteractiveObj = Cast<IInteractable>(ObjData.HitActor);
	//		InteractiveObj->Execute_OnInteract(ObjData.HitActor, GetController(), ObjData.HitComponent);
	//	}
	//}
}

void AOBM_PlayerCharacter::ThrowEquippedObject()
{
	//if (InventoryComponent && ItemHoldingPoint)
	//{
	//	AActor* ActorToThrow = InventoryComponent->DropSelectedItem();

	//	if (!ActorToThrow) return;

	//	// Set Object location to throwing point location, before throwing it
	//	//ActorToThrow->SetActorLocation(ItemHoldingPoint->GetComponentLocation(), false, nullptr, ETeleportType::TeleportPhysics);

	//	UStaticMeshComponent* ActorMesh = Cast<UStaticMeshComponent>(ActorToThrow->GetComponentByClass(UStaticMeshComponent::StaticClass()));

	//	if (ActorMesh)
	//	{
	//		ActorMesh->SetSimulatePhysics(true);
	//		FVector Location;
	//		FRotator Rotation;
	//		GetActorEyesViewPoint(Location, Rotation);
	//		ActorMesh->SetAllPhysicsLinearVelocity(Rotation.Vector() * ThrowForce);

	//	}
	//}
}

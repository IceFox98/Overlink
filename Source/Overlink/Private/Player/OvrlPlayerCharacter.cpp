// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/OvrlPlayerCharacter.h"

#include "Player/Components/OvrlCameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/Components/OvrlHealthComponent.h"
#include "Player/Components/OvrlInteractionComponent.h"
#include "Player/Components/OvrlParkourComponent.h"
#include "Player/Input/OvrlInputComponent.h"
#include "Player/Input/OvrlInputConfig.h"
#include "Inventory/OvrlInventoryComponent.h"
#include "AbilitySystem/OvrlAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/OvrlHealthSet.h"
#include "Player/Components/OvrlCharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "Components/StaticMeshComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayEffectTypes.h"

#include "OvrlUtils.h"

AOvrlPlayerCharacter::AOvrlPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UOvrlCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	//SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	//SpringArm->TargetArmLength = 0.f;
	//SpringArm->bUsePawnControlRotation = true;
	//SpringArm->SetupAttachment(GetMesh(), TEXT("camera"));

	// Create a follow camera
	CameraComp = CreateDefaultSubobject<UOvrlCameraComponent>(TEXT("CameraComp"));
	CameraComp->bUsePawnControlRotation = true; // Camera does not rotate relative to arm
	CameraComp->SetupAttachment(RootComponent);

	FPMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FPMesh"));
	FPMesh->SetupAttachment(CameraComp);

	InteractionComponent = CreateDefaultSubobject<UOvrlInteractionComponent>(TEXT("InteractionComponent"));
	InventoryComponent = CreateDefaultSubobject<UOvrlInventoryComponent>(TEXT("InventoryComponent"));
	ParkourComponent = CreateDefaultSubobject<UOvrlParkourComponent>(TEXT("ParkourComponent"));

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanJump = true;

	GetCharacterMovement()->bCanWalkOffLedgesWhenCrouching = true;

	ThrowForce = 1200.f;
}

void AOvrlPlayerCharacter::BeginPlay()
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

void AOvrlPlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	UOvrlInputComponent* OvrlIC = Cast<UOvrlInputComponent>(PlayerInputComponent);

	if (ensureMsgf(OvrlIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UOvrlInputComponent or a subclass of it.")))
	{
		// This is where we actually bind and input action to a gameplay tag, which means that Gameplay Ability Blueprints will
		// be triggered directly by these input actions Triggered events. 
		TArray<uint32> BindHandles;
		OvrlIC->BindAbilityActions(InputConfig, this, &ThisClass::OnAbilityInputPressed, &ThisClass::OnAbilityInputReleased, /*out*/ BindHandles);

		OvrlIC->BindNativeAction(InputConfig, OvrlInputTags::Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
		OvrlIC->BindNativeAction(InputConfig, OvrlInputTags::Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
	}
}

void AOvrlPlayerCharacter::OnAbilityInputPressed(FGameplayTag InputTag)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityInputTagPressed(InputTag);
	}
}

void AOvrlPlayerCharacter::OnAbilityInputReleased(FGameplayTag InputTag)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityInputTagReleased(InputTag);
	}
}

void AOvrlPlayerCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

	if (Value.X != 0.0f)
	{
		if (true)
		{
			MovementRotation = UOvrlUtils::GetGravityRelativeRotation(Controller->GetControlRotation(), GetCharacterMovement()->GetGravityDirection());
			MovementRotation.Pitch = 0.f;
			MovementRotation = UOvrlUtils::GetGravityWorldRotation(MovementRotation, GetCharacterMovement()->GetGravityDirection());
		}

		const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
		AddMovementInput(MovementDirection, Value.X);
	}

	if (Value.Y != 0.0f)
	{
		if (true)
		{
			MovementRotation = UOvrlUtils::GetGravityRelativeRotation(Controller->GetControlRotation(), GetCharacterMovement()->GetGravityDirection());
			MovementRotation.Roll = 0.f;
			MovementRotation.Pitch = 0.f;

			MovementRotation = UOvrlUtils::GetGravityWorldRotation(MovementRotation, GetCharacterMovement()->GetGravityDirection());
		}

		const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
		AddMovementInput(MovementDirection, Value.Y);
	}
}

void AOvrlPlayerCharacter::Input_LookMouse(const FInputActionValue& InputActionValue)
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

void AOvrlPlayerCharacter::Crouch(bool bClientSimulation/* = false*/)
{
	Super::Crouch();

	SetStance(OvrlStanceTags::Crouching);
}

void AOvrlPlayerCharacter::UnCrouch(bool bClientSimulation/* = false*/)
{
	Super::UnCrouch();

	SetStance(OvrlStanceTags::Standing);
}

void AOvrlPlayerCharacter::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	// Use the character movement mode to set the locomotion mode to the right value. This allows you to have a
	// custom set of movement modes but still use the functionality of the default character movement component.

	switch (GetCharacterMovement()->MovementMode)
	{
	case MOVE_Walking:
	case MOVE_NavWalking:
		SetLocomotionMode(OvrlLocomotionModeTags::Grounded);
		break;

	case MOVE_Falling:
		SetLocomotionMode(OvrlLocomotionModeTags::InAir);
		break;

	default:
		SetLocomotionMode(FGameplayTag::EmptyTag);
		break;
	}

	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}

void AOvrlPlayerCharacter::SetLocomotionAction(const FGameplayTag& NewLocomotionAction)
{
	if (LocomotionAction == NewLocomotionAction)
		return;

	LocomotionAction = NewLocomotionAction;
}

void AOvrlPlayerCharacter::SetLocomotionMode(const FGameplayTag& NewLocomotionMode)
{
	if (LocomotionMode == NewLocomotionMode)
		return;

	LocomotionMode = NewLocomotionMode;
}

void AOvrlPlayerCharacter::SetStance(const FGameplayTag& NewStance)
{
	if (Stance == NewStance)
		return;

	Stance = NewStance;
}

void AOvrlPlayerCharacter::SetGait(const FGameplayTag& NewGait)
{
	if (Gait == NewGait)
		return;

	Gait = NewGait;
}

void AOvrlPlayerCharacter::SetOverlayMode(const FGameplayTag& NewOverlayMode)
{
	if (OverlayMode == NewOverlayMode)
		return;

	OverlayMode = NewOverlayMode;
}

void AOvrlPlayerCharacter::Interact()
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

void AOvrlPlayerCharacter::ThrowEquippedObject()
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

// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/Components/HealthComponent.h"
#include "Player/Components/InteractionComponent.h"
#include "Player/Components/InventoryComponent.h"
#include "AbilitySystem/OBM_AbilitySet.h"
#include "AbilitySystem/OBM_AbilitySystemComponent.h"
#include "Player/Input/OBM_InputComponent.h"
#include "Player/Input/OBM_InputConfig.h"
#include "Components/StaticMeshComponent.h"
#include "../OBM_GameplayTags.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bAbilitiesInitialized = false;

	// Create a follow camera
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(RootComponent);
	CameraComp->bUsePawnControlRotation = true; // Camera does not rotate relative to arm

	ItemHoldingPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ItemHoldingPoint"));
	ItemHoldingPoint->SetupAttachment(CameraComp);

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	//InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	AbilitySystemComponent = CreateDefaultSubobject<UOBM_AbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	//Attributes = CreateDefaultSubobject<UPuzzleGuyAttributeSet>(TEXT("Attributes"));

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanJump = true;

	ThrowForce = 1200.f;
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	UOBM_InputComponent* OBM_IC = Cast<UOBM_InputComponent>(PlayerInputComponent);

	if (ensureMsgf(OBM_IC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UOBM_InputComponent or a subclass of it.")))
	{
		// This is where we actually bind and input action to a gameplay tag, which means that Gameplay Ability Blueprints will
		// be triggered directly by these input actions Triggered events. 
		TArray<uint32> BindHandles;
		OBM_IC->BindAbilityActions(InputConfig, this, &ThisClass::OnAbilityInputPressed, &ThisClass::OnAbilityInputReleased, /*out*/ BindHandles);

		OBM_IC->BindNativeAction(InputConfig, OBM_GameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
		OBM_IC->BindNativeAction(InputConfig, OBM_GameplayTags::InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
		//OBM_IC->BindNativeAction(InputConfig, OBM_GameplayTags::InputTag_Crouch, ETriggerEvent::Triggered, this, &ThisClass::Input_Crouch, /*bLogIfNotFound=*/ false);
	}
}


void APlayerCharacter::OnAbilityInputPressed(FGameplayTag InputTag)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityInputTagPressed(InputTag);
	}
}

void APlayerCharacter::OnAbilityInputReleased(FGameplayTag InputTag)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityInputTagReleased(InputTag);
	}
}

//void APlayerCharacter::OnHealthChanged(UHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
//{
//	Super::OnHealthChanged(OwningHealthComp, Health, HealthDelta, DamageType, InstigatedBy, DamageCauser);
//
//	if (HealthComp->IsDead())
//	{
//		UE_LOG(LogTemp, Warning, TEXT("You're dead!"));
//		return;
//	}
//
//	// Do other stuff
//}

void APlayerCharacter::HandleHealthChanged(float DeltaValue, const FGameplayTagContainer& EventTags)
{
	if (bAbilitiesInitialized)
	{
		OnHealthChanged(DeltaValue, EventTags);
	}
}

void APlayerCharacter::Input_Move(const FInputActionValue& InputActionValue)
{
	const FVector2D Value = InputActionValue.Get<FVector2D>();

	const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

	if (Value.X != 0.0f)
	{
		const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
		AddMovementInput(MovementDirection, Value.X);
	}

	if (Value.Y != 0.0f)
	{
		const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
		AddMovementInput(MovementDirection, Value.Y);
	}
}

void APlayerCharacter::Input_LookMouse(const FInputActionValue& InputActionValue)
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

void APlayerCharacter::Interact()
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

void APlayerCharacter::ThrowEquippedObject()
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

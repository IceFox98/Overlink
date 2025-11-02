// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/OvrlPlayerCharacter.h"

// Internal
#include "Player/Components/OvrlCameraComponent.h"
#include "Player/Components/OvrlHealthComponent.h"
#include "Player/Components/OvrlInteractionComponent.h"
#include "Player/Input/OvrlInputComponent.h"
#include "Player/Input/OvrlInputConfig.h"
#include "Inventory/OvrlInventoryComponent.h"
#include "AbilitySystem/OvrlAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/OvrlHealthSet.h"
#include "Player/Components/OvrlCharacterMovementComponent.h"
#include "OvrlUtils.h"

// Engine
#include "GameFramework/CharacterMovementComponent.h"
#include "Animations/OvrlLinkedAnimInstance.h"
#include "MotionWarpingComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayEffectTypes.h"
#include "Engine/StaticMeshActor.h"


AOvrlPlayerCharacter::AOvrlPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UOvrlCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	// Create a follow camera
	CameraComp = CreateDefaultSubobject<UOvrlCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(GetMesh(), TEXT("head"));
	CameraComp->bUsePawnControlRotation = true; // Camera does not rotate relative to arm
	CameraComp->FirstPersonScale = .2f; // Used to avoid arms compenetrating walls when too close
	CameraComp->bEnableFirstPersonScale = true;

	GetMesh()->CastShadow = false;
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson; // Used to avoid arms compenetrating walls when too close
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetDisablePostProcessBlueprint(true);

	FullBodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FullBodyMesh"));
	FullBodyMesh->SetupAttachment(RootComponent);
	FullBodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FullBodyMesh->SetDisablePostProcessBlueprint(true);

	InteractionComponent = CreateDefaultSubobject<UOvrlInteractionComponent>(TEXT("InteractionComponent"));
	InventoryComponent = CreateDefaultSubobject<UOvrlInventoryComponent>(TEXT("InventoryComponent"));
	MotionWarping = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarping"));

	ThrowForce = 1200.f;
}

void AOvrlPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	//ParkourComponent->Initialize(this);

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
		OvrlIC->BindNativeAction(InputConfig, OvrlInputTags::Crouch, ETriggerEvent::Started, this, &ThisClass::Input_Crouch, /*bLogIfNotFound=*/ false);
		OvrlIC->BindNativeAction(InputConfig, OvrlInputTags::Run, ETriggerEvent::Started, this, &ThisClass::Input_StartRun, /*bLogIfNotFound=*/ false);
		OvrlIC->BindNativeAction(InputConfig, OvrlInputTags::Run, ETriggerEvent::Completed, this, &ThisClass::Input_EndRun, /*bLogIfNotFound=*/ false);
	}
}

UOvrlCharacterMovementComponent* AOvrlPlayerCharacter::GetCharacterMovement() const
{
	return Cast<UOvrlCharacterMovementComponent>(GetMovementComponent());
}

void AOvrlPlayerCharacter::ApplyAnimLayerClass(const TSubclassOf<UOvrlLinkedAnimInstance>& LayerClass)
{
	if (GetMesh() && FullBodyMesh)
	{
		GetMesh()->LinkAnimClassLayers(LayerClass);
		FullBodyMesh->LinkAnimClassLayers(LayerClass);
	}
}

void AOvrlPlayerCharacter::RestoreAnimLayerClass()
{
	if (GetMesh() && FullBodyMesh)
	{
		GetMesh()->LinkAnimClassLayers(DefaultAnimLayerClass);
		FullBodyMesh->LinkAnimClassLayers(DefaultAnimLayerClass);
	}
}

void AOvrlPlayerCharacter::EquipObject(AActor* ObjectToEquip, UStaticMesh* MeshToDisplay)
{
	Super::EquipObject(ObjectToEquip, MeshToDisplay);

	ensure(MeshToDisplay);

	// Spawn static mesh that is only used to cast shadows
	if (!IsValid(EquippedObjectMesh))
	{
		EquippedObjectMesh = GetWorld()->SpawnActor<AStaticMeshActor>();
		EquippedObjectMesh->SetMobility(EComponentMobility::Movable);
		EquippedObjectMesh->SetActorEnableCollision(false);
		EquippedObjectMesh->AttachToComponent(FullBodyMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, GripPointName);
		EquippedObjectMesh->SetActorHiddenInGame(true);
	}

	if (EquippedObjectMesh)
	{
		EquippedObjectMesh->GetStaticMeshComponent()->SetStaticMesh(MeshToDisplay);

		// Setup invisible mesh, able to cast shadows
		EquippedObjectMesh->GetStaticMeshComponent()->CastShadow = true;
		EquippedObjectMesh->GetStaticMeshComponent()->bCastHiddenShadow = true;
	}
}

void AOvrlPlayerCharacter::UnequipObject()
{
	Super::UnequipObject();

	if (EquippedObjectMesh)
	{
		EquippedObjectMesh->GetStaticMeshComponent()->bCastHiddenShadow = false;
	}
}

void AOvrlPlayerCharacter::PlayAnimMontage(UAnimMontage* MontageToPlay, float StartTime/* = 0.f*/)
{
	Super::PlayAnimMontage(MontageToPlay, StartTime);

	FullBodyMesh->GetAnimInstance()->Montage_Play(MontageToPlay, 1.f, EMontagePlayReturnType::Duration, StartTime);
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

void AOvrlPlayerCharacter::Input_Crouch(const FInputActionValue& InputActionValue)
{
	GetCharacterMovement()->HandleCrouching(!bIsCrouched);
}

void AOvrlPlayerCharacter::Input_StartRun(const FInputActionValue& InputActionValue)
{
	GetCharacterMovement()->TryStartRunning();
}

void AOvrlPlayerCharacter::Input_EndRun(const FInputActionValue& InputActionValue)
{
	GetCharacterMovement()->StopRunning();
}

void AOvrlPlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	FVector& MeshRelativeLocation = FullBodyMesh->GetRelativeLocation_DirectMutable();
	MeshRelativeLocation.Z = BaseTranslationOffset.Z;
}

void AOvrlPlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	FVector& MeshRelativeLocation = FullBodyMesh->GetRelativeLocation_DirectMutable();
	MeshRelativeLocation.Z = BaseTranslationOffset.Z;
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

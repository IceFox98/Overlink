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
#include "Animations/OvrlLinkedAnimInstance.h"
#include "MotionWarpingComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "Components/StaticMeshComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayEffectTypes.h"

#include "OvrlUtils.h"

AOvrlPlayerCharacter::AOvrlPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UOvrlCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetMesh(), TEXT("head"));

	// Create a follow camera
	CameraComp = CreateDefaultSubobject<UOvrlCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArm);
	CameraComp->bUsePawnControlRotation = true; // Camera does not rotate relative to arm


	//FullBodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FullBodyMesh"));
	//FullBodyMesh->SetupAttachment(RootComponent);
	//FullBodyMesh->bOwnerNoSee = true;
	//FullBodyMesh->bCastHiddenShadow = true;
	//FullBodyMesh->SetDisablePostProcessBlueprint(true);

	InteractionComponent = CreateDefaultSubobject<UOvrlInteractionComponent>(TEXT("InteractionComponent"));
	InventoryComponent = CreateDefaultSubobject<UOvrlInventoryComponent>(TEXT("InventoryComponent"));
	MotionWarping = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarping"));

	GetMesh()->CastShadow = false;

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
	}
}

void AOvrlPlayerCharacter::ApplyAnimClassLayer(const TSubclassOf<UOvrlLinkedAnimInstance>& LayerClass)
{
	GetMesh()->LinkAnimClassLayers(LayerClass);
}

void AOvrlPlayerCharacter::PlayAnimMontage(UAnimMontage* MontageToPlay, float StartTime/* = 0.f*/)
{
	GetMesh()->GetAnimInstance()->Montage_Play(MontageToPlay, 1.f, EMontagePlayReturnType::Duration, StartTime);
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

}

void AOvrlPlayerCharacter::UnCrouch(bool bClientSimulation/* = false*/)
{
	Super::UnCrouch();
}

void AOvrlPlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
	MeshRelativeLocation.Z = BaseTranslationOffset.Z;
}

void AOvrlPlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
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

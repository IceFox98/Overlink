// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/OvrlPlayerCharacter.h"

// Internal
#include "Player/Components/OvrlCameraComponent.h"
#include "Player/Components/OvrlInteractionComponent.h"
#include "Player/Input/OvrlInputComponent.h"
#include "Player/Input/OvrlInputConfig.h"
#include "Player/Components/OvrlCharacterMovementComponent.h"
#include "Player/Components/OvrlEquipmentManagerComponent.h"
#include "Inventory/OvrlInventoryComponent.h"
#include "AbilitySystem/OvrlAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/OvrlHealthSet.h"
#include "Audio/OvrlFoleyAudioBank.h"
#include "Core/Interfaces/OvrlInteractable.h"
#include "OvrlUtils.h"

// Engine
#include "GameFramework/CharacterMovementComponent.h"
#include "Animations/OvrlLinkedAnimInstance.h"
#include "MotionWarpingComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayEffectTypes.h"
#include "Engine/StaticMeshActor.h"
#include "Components/CapsuleComponent.h"
#include "KismetTraceUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

AOvrlPlayerCharacter::AOvrlPlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	// Create a follow camera
	CameraComp = CreateDefaultSubobject<UOvrlCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(GetMesh(), TEXT("head"));
	CameraComp->bUsePawnControlRotation = true; // Camera does not rotate relative to arm
	CameraComp->FirstPersonScale = .2f; // Used to avoid arms compenetrating walls when too close
	CameraComp->bEnableFirstPersonScale = true;

	// Disable controller rotation since it was dealing with pawn rotation when a different vector gravity is applied.
	// Let the Movement Component handle the rotation.
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->RotationRate = FRotator(-1.f, -1.f, -1.f); // Set to negative to have instant turns of the player

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
	EquipmentManagerComponent = CreateDefaultSubobject<UOvrlEquipmentManagerComponent>(TEXT("EquipmentManagerComponent"));
	MotionWarping = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarping"));

	bEnableCameraStabilization = true;
	LandSoundMultiplier = .5f;
	JumpSoundMultiplier = .5f;
	SlideSoundMultiplier = .5f;
}

void AOvrlPlayerCharacter::OnJumped_Implementation()
{
	Super::OnJumped_Implementation();
	OnPlayerJumped.Broadcast();

	PlayJumpSound();
}

void AOvrlPlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	PlayLandSound();
}

void AOvrlPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	// Need to initialize before BeginPlay to make sure the delegates are bound.
	EquipmentManagerComponent->InitializeFromInventory(InventoryComponent);
}

void AOvrlPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	GetCharacterMovement()->OnLocomotionActionChanged.AddUniqueDynamic(this, &AOvrlPlayerCharacter::OnLocomotionActionChanged);
}

bool AOvrlPlayerCharacter::CanJumpInternal_Implementation() const
{
	return Super::CanJumpInternal_Implementation() || GetCharacterMovement()->IsWallrunning() || GetCharacterMovement()->IsWallClinging();
}

void AOvrlPlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UOvrlInputComponent* OvrlIC = Cast<UOvrlInputComponent>(PlayerInputComponent);

	if (ensureMsgf(OvrlIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UOvrlInputComponent or a subclass of it.")))
	{
		// This is where we actually bind and input action to a gameplay tag, which means that Gameplay Ability Blueprints will
		// be triggered directly by these input actions Triggered events. 
		TArray<uint32> BindHandles;
		OvrlIC->BindAbilityActions(InputConfig, this, &ThisClass::OnAbilityInputStarted, &ThisClass::OnAbilityInputTriggered, &ThisClass::OnAbilityInputReleased, /*out*/ BindHandles);

		OvrlIC->BindNativeAction(InputConfig, OvrlInputTags::Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
		OvrlIC->BindNativeAction(InputConfig, OvrlInputTags::Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
		OvrlIC->BindNativeAction(InputConfig, OvrlInputTags::Crouch, ETriggerEvent::Started, this, &ThisClass::Input_Crouch, /*bLogIfNotFound=*/ false);
		OvrlIC->BindNativeAction(InputConfig, OvrlInputTags::Interact, ETriggerEvent::Started, this, &ThisClass::Input_StartInteract, /*bLogIfNotFound=*/ false);
		OvrlIC->BindNativeAction(InputConfig, OvrlInputTags::Interact, ETriggerEvent::Completed, this, &ThisClass::Input_EndInteract, /*bLogIfNotFound=*/ false);
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

		//if (!CheckWallCollisions(MovementDirection * Value.X))
		{
			AddMovementInput(MovementDirection, Value.X);
		}
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

		//if (!CheckWallCollisions(MovementDirection * Value.Y))
		{
			AddMovementInput(MovementDirection, Value.Y);
		}
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

void AOvrlPlayerCharacter::Input_StartInteract(const FInputActionValue& InputActionValue)
{
	if (InteractionComponent && InteractionComponent->GetCurrentPointedObjectData().OriginalObject)
	{
		IOvrlInteractable::Execute_Interact(InteractionComponent->GetCurrentPointedObjectData().OriginalObject);
	}
}

void AOvrlPlayerCharacter::Input_EndInteract(const FInputActionValue& InputActionValue)
{
	if (InteractionComponent && InteractionComponent->GetCurrentPointedObjectData().OriginalObject)
	{
		IOvrlInteractable::Execute_EndInteract(InteractionComponent->GetCurrentPointedObjectData().OriginalObject);
	}
}

void AOvrlPlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	FVector& MeshRelativeLocation = FullBodyMesh->GetRelativeLocation_DirectMutable();
	MeshRelativeLocation.Z = MeshRelativeLocation.Z + HalfHeightAdjust;
}

void AOvrlPlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	const AOvrlPlayerCharacter* DefaultChar = GetDefault<AOvrlPlayerCharacter>(GetClass());
	if (FullBodyMesh && DefaultChar->FullBodyMesh)
	{
		FVector& MeshRelativeLocation = FullBodyMesh->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = DefaultChar->FullBodyMesh->GetRelativeLocation().Z;
	}
}

void AOvrlPlayerCharacter::PlayLandSound() const
{
	if (!FoleyAudioBank) return;

	const float PlayerVelocityZ = GetCharacterMovement()->GetRelativeLastUpdateVelocity().Z;
	constexpr float SoundVelocityThreshold = 200.f;

	if (PlayerVelocityZ < -SoundVelocityThreshold)
	{
		USoundBase* LandSound = FoleyAudioBank->GetSound(OvrlFoleyEvents::Land, SurfaceType_Default);
		UGameplayStatics::PlaySoundAtLocation(this, LandSound, FullBodyMesh->GetComponentLocation(), LandSoundMultiplier);
	}
}

void AOvrlPlayerCharacter::PlayJumpSound() const
{
	if (!FoleyAudioBank) return;

	USoundBase* JumpSound = FoleyAudioBank->GetSound(OvrlFoleyEvents::Jump, SurfaceType_Default);
	UGameplayStatics::PlaySoundAtLocation(this, JumpSound, FullBodyMesh->GetComponentLocation(), JumpSoundMultiplier);
}

void AOvrlPlayerCharacter::OvrlPlayAnimMontage(UAnimMontage* MontageToPlay, float StartTime/* = 0.f*/)
{
	Super::OvrlPlayAnimMontage(MontageToPlay, StartTime);

	FullBodyMesh->GetAnimInstance()->Montage_Play(MontageToPlay, 1.f, EMontagePlayReturnType::Duration, StartTime);
}

void AOvrlPlayerCharacter::OvrlStopAnimMontage(UAnimMontage* MontageToStop)
{
	Super::OvrlStopAnimMontage(MontageToStop);

	if (MontageToStop)
	{
		FullBodyMesh->GetAnimInstance()->Montage_Stop(MontageToStop->BlendOut.GetBlendTime(), MontageToStop);
	}
}

void AOvrlPlayerCharacter::ApplyAnimLayerClass(const TSubclassOf<UOvrlLinkedAnimInstance>& LayerClass)
{
	Super::ApplyAnimLayerClass(LayerClass);

	if (FullBodyMesh)
	{
		FullBodyMesh->LinkAnimClassLayers(LayerClass);
	}
}

void AOvrlPlayerCharacter::RestoreAnimLayerClass()
{
	Super::RestoreAnimLayerClass();

	if (FullBodyMesh)
	{
		FullBodyMesh->LinkAnimClassLayers(DefaultAnimLayerClass);
	}
}

void AOvrlPlayerCharacter::EquipObject(AActor* ObjectToEquip, FName AttachSocketName, UStaticMesh* MeshToDisplay)
{
	Super::EquipObject(ObjectToEquip, AttachSocketName, MeshToDisplay);

	ensure(MeshToDisplay);

	// Spawn static mesh that is only used to cast shadows
	if (!IsValid(EquippedObjectMesh))
	{
		EquippedObjectMesh = GetWorld()->SpawnActor<AStaticMeshActor>();
		EquippedObjectMesh->SetMobility(EComponentMobility::Movable);
		EquippedObjectMesh->SetActorEnableCollision(false);
		EquippedObjectMesh->AttachToComponent(FullBodyMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachSocketName);
		EquippedObjectMesh->SetActorHiddenInGame(true);
	}

	if (EquippedObjectMesh)
	{
		EquippedObjectMesh->GetStaticMeshComponent()->SetStaticMesh(MeshToDisplay);
		EquippedObjectMesh->GetStaticMeshComponent()->SetVisibility(false, true);

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
		EquippedObjectMesh->MarkComponentsRenderStateDirty();
	}
}

void AOvrlPlayerCharacter::OnAbilityInputStarted(FGameplayTag InputTag)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityInputTagStarted(InputTag);
	}
}

void AOvrlPlayerCharacter::OnAbilityInputTriggered(FGameplayTag InputTag)
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

void AOvrlPlayerCharacter::OnLocomotionActionChanged(const FGameplayTag& OldLocomotionAction, const FGameplayTag& NewLocomotionAction)
{
	if (!FoleyAudioBank) return;

	if (NewLocomotionAction == OvrlLocomotionActionTags::Sliding)
	{
		USoundBase* SlideSound = FoleyAudioBank->GetSound(OvrlFoleyEvents::Slide, SurfaceType_Default);
		SlidingAudioComponent = UGameplayStatics::SpawnSoundAttached(SlideSound, FullBodyMesh);
		SlidingAudioComponent->SetVolumeMultiplier(SlideSoundMultiplier);
	}
	else if (OldLocomotionAction == OvrlLocomotionActionTags::Sliding && SlidingAudioComponent)
	{
		SlidingAudioComponent->FadeOut(.5f, 0.f);
	}
}

bool AOvrlPlayerCharacter::CheckWallCollisions(const FVector& Direction)
{
	//const float TraceLength = GetCapsuleComponent()->GetScaledCapsuleRadius();
	const float TraceLength = 50.f;
	const FVector TraceStart = GetCharacterMovement()->GetActorFeetLocation() - GetGravityDirection() * 40.f;
	const FVector TraceEnd = TraceStart + Direction * TraceLength;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bFindInitialOverlaps = false;

	FHitResult OutHit;
	GetWorld()->LineTraceSingleByChannel(OutHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams);

	DrawDebugLineTraceSingle(GetWorld(), TraceStart, TraceEnd, EDrawDebugTrace::ForOneFrame, OutHit.bBlockingHit, OutHit, FLinearColor::Red, FLinearColor::Green, -1.f);

	const float WallAngle = 10.f;

	if (OutHit.bBlockingHit)
	{
		const float WallAngleDot = FMath::Cos(FMath::DegreesToRadians(WallAngle));
		const float Dot = FVector::DotProduct(Direction, -OutHit.ImpactNormal);

		return Dot > WallAngleDot;
	}

	return false;
}

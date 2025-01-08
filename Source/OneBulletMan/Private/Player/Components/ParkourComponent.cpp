// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Components/ParkourComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "OBM_Utils.h"

// Sets default values for this component's properties
UParkourComponent::UParkourComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	WallrunCheckDistance = 75.f;
	WallrunCheckAngle = 35.f;
	WallrunCameraTiltAngle = 15.f;
	WallrunResetTime = .35f;
	WallrunJumpVelocity = FVector(1000.f, 1000.f, 800.f);

	SlideVectorCheck = FVector(0.f, 0.f, -200.f);
	SlideForce = 800.f;
	SlideGroundFriction = 0.f;
	SlideBraking = 1000.f;
	SlideMaxWalkSpeedCrouched = 0.f;
}

// Called when the game starts
void UParkourComponent::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void UParkourComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	HandleWallrun(DeltaTime);

	if (ShouldCancelSliding())
	{
		// TODO: Replace with Gameplay Ability
		Character->UnCrouch();
		CancelSliding();
	}
}

void UParkourComponent::Initialize(ACharacter* InCharacter)
{
	check(InCharacter);

	Character = InCharacter;
	CharacterMovement = Character->GetCharacterMovement();

	// Save original character values due to reset them after applying parkour movement.
	DefaultGravity = CharacterMovement->GravityScale;
	DefaultMaxWalkSpeed = CharacterMovement->MaxWalkSpeed;
	DefaultMaxWalkSpeedCrouched = CharacterMovement->MaxWalkSpeedCrouched;
	DefaultGroundFriction = CharacterMovement->GroundFriction;
	DefaultBrakingDecelerationWalking = CharacterMovement->BrakingDecelerationWalking;
}

void UParkourComponent::OnPlayerJumped()
{
	bShouldSlideOnLanded = false;

	if (MovementType == EParkourMovementType::None)
	{
		// The player will still be on the ground (not falling) when the Jump function is called
		if (!CharacterMovement->IsFalling())
		{
			bCanCheckWallrun = true;
		}
	}
	else
	{
		HandleWallrunJump();
	}
}

void UParkourComponent::OnPlayerLanded()
{
	EndWallrun();

	if (bShouldSlideOnLanded)
	{
		Character->Crouch();

		// Is Player still moving?
		if (CharacterMovement->GetLastUpdateVelocity().Length() > 0)
		{
			HandleSliding();
		}

		bShouldSlideOnLanded = false;
	}
}

void UParkourComponent::HandleCrouching(bool bWantsToCrouch)
{
	const bool bIsPlayerGrounded = !CharacterMovement->IsFalling();

	if (bWantsToCrouch)
	{
		if (bIsPlayerGrounded)
		{
			Character->Crouch();
		}

		HandleSliding();
	}
	else
	{
		Character->UnCrouch();

		CancelSliding();
	}
}

void UParkourComponent::HandleWallrun(float DeltaTime)
{
	if (bCanCheckWallrun)
	{
		const FVector PlayerInputVector = CharacterMovement->GetLastInputVector();
		const FVector PlayerForwardVector = Character->GetActorForwardVector();

		const bool bIsPlayerNotGrounded = CharacterMovement->IsFalling();
		//const bool bIsPlayerMoving = !PlayerInputVector.IsZero();
		const bool bIsPlayerMovingForward = FVector::DotProduct(PlayerInputVector, PlayerForwardVector) > 0;

		if (bIsPlayerNotGrounded && bIsPlayerMovingForward)
		{
			if (HandleWallrunMovement(true))
			{
				MovementType = EParkourMovementType::WallrunLeft;
			}
			else if(HandleWallrunMovement(false)) // Try the right side
			{
				MovementType = EParkourMovementType::WallrunRight;
			}
		}
	}
	
	HandleWallrunCameraTilt(DeltaTime);
}

bool UParkourComponent::HandleWallrunMovement(bool bIsLeftSide)
{
	const float WallDirection = bIsLeftSide ? -1.f : 1.f;

	const FVector BackwardVector = (Character->GetActorRightVector() * WallrunCheckDistance * WallDirection) + Character->GetActorForwardVector() * -WallrunCheckAngle;

	const FVector StartTrace = Character->GetActorLocation();
	const FVector EndTrace = Character->GetActorLocation() + BackwardVector;

	FHitResult OutHit;
	UKismetSystemLibrary::LineTraceSingle(this, StartTrace, EndTrace, ETraceTypeQuery::TraceTypeQuery1, false, {}, EDrawDebugTrace::None, OutHit, true);

	bool bHandled = false;

	if (OutHit.bBlockingHit)
	{
		WallrunNormal = OutHit.Normal;

		// Returns the direction of where the player should be launched. It follows the wall surface.
		const FVector WallForwardDirection = FVector::CrossProduct(WallrunNormal, FVector::UpVector);
		const FVector LaunchVelocity = WallForwardDirection * DefaultMaxWalkSpeed * -WallDirection;

		Character->LaunchCharacter(LaunchVelocity, true, true);
		bHandled = true;
	}

	return bHandled;
}

void UParkourComponent::HandleWallrunCameraTilt(float DeltaTime)
{
	FRotator TargetRotation = Character->GetControlRotation();

	if (IsWallrunning())
	{
		// Tilt camera depending on which wall the player is on.
		TargetRotation.Roll = MovementType == EParkourMovementType::WallrunLeft ? WallrunCameraTiltAngle : -WallrunCameraTiltAngle;
	}
	else
	{
		//TargetRotation.Roll = 0.f;
	}

	// Lerp and apply rotation
	const FRotator FinalRotation = UKismetMathLibrary::RInterpTo(Character->GetControlRotation(), TargetRotation, DeltaTime, 10.f);
	Character->GetController()->SetControlRotation(FinalRotation);
}

void UParkourComponent::HandleWallrunJump()
{
	if (IsWallrunning())
	{
		EndWallrun();

		const FVector LaunchVelocity = FVector(WallrunNormal.X, WallrunNormal.Y, 1.f) * WallrunJumpVelocity;
		Character->LaunchCharacter(LaunchVelocity, false, true);
	}
}

void UParkourComponent::ResetWallrun()
{
	if (MovementType == EParkourMovementType::None)
	{
		bCanCheckWallrun = true;
	}
}

void UParkourComponent::EndWallrun()
{
	if (IsWallrunning())
	{
		bCanCheckWallrun = false;

		MovementType = EParkourMovementType::None;
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UParkourComponent::ResetWallrun, WallrunResetTime, false);
	}
}

void UParkourComponent::HandleSliding()
{
	if (IsSliding())
		return;

	const bool bIsPlayerGrounded = !CharacterMovement->IsFalling();
	const bool bIsPlayerMoving = CharacterMovement->GetLastUpdateVelocity().Length() > 0;

	bShouldSlideOnLanded = !bIsPlayerGrounded;

	if (bIsPlayerGrounded && bIsPlayerMoving)
	{

		// Trace a down vector to check if sliding is available.
		const FVector StartTrace = Character->GetActorLocation();
		const FVector EndTrace = StartTrace + SlideVectorCheck;

		FHitResult OutHit;
		UKismetSystemLibrary::LineTraceSingle(this, StartTrace, EndTrace, ETraceTypeQuery::TraceTypeQuery1, false, {}, EDrawDebugTrace::ForDuration, OutHit, true);

		if (OutHit.bBlockingHit)
		{
			CharacterMovement->GroundFriction = SlideGroundFriction;
			CharacterMovement->BrakingDecelerationWalking = SlideBraking;
			CharacterMovement->MaxWalkSpeedCrouched = SlideMaxWalkSpeedCrouched;

			// Cross with floor normal to get the direction of where to launche the character
			const FVector FloorSlopeDirection = FVector::CrossProduct(Character->GetActorRightVector(), OutHit.Normal);
			CharacterMovement->AddImpulse(FloorSlopeDirection * SlideForce, true);

			MovementType = EParkourMovementType::Sliding;
		}
	}
}

bool UParkourComponent::ShouldCancelSliding()
{
	if (!IsSliding())
		return false;

	//OBM_LOG("%f", CharacterMovement->GetLastUpdateVelocity().Length());

	return CharacterMovement->GetLastUpdateVelocity().Length() <= 575.f;
}

void UParkourComponent::CancelSliding()
{
	CharacterMovement->GroundFriction = DefaultGroundFriction;
	CharacterMovement->BrakingDecelerationWalking = DefaultBrakingDecelerationWalking;
	CharacterMovement->MaxWalkSpeedCrouched = DefaultMaxWalkSpeedCrouched;

	MovementType = EParkourMovementType::None;
}

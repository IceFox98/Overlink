// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Components/OvrlParkourComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

#include "OvrlUtils.h"

// Sets default values for this component's properties
UOvrlParkourComponent::UOvrlParkourComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	WallrunCheckDistance = 75.f;
	WallrunCheckAngle = 35.f;
	WallrunCameraTiltAngle = 15.f;
	WallrunResetTime = .35f;
	WallrunJumpVelocity = FVector(1000.f, 1000.f, 800.f);

	SlideDistanceCheck = 200.f;
	SlideForce = 800.f;
	SlideGroundFriction = 0.f;
	SlideBraking = 1000.f;
	SlideMaxWalkSpeedCrouched = 0.f;
}

// Called every frame
void UOvrlParkourComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
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

void UOvrlParkourComponent::Initialize(ACharacter* InCharacter)
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

void UOvrlParkourComponent::OnPlayerJumped()
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

void UOvrlParkourComponent::OnPlayerLanded()
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

void UOvrlParkourComponent::HandleCrouching(bool bWantsToCrouch)
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

void UOvrlParkourComponent::HandleWallrun(float DeltaTime)
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
	
	// The tilting is now managed in the PlayerController class
	//HandleWallrunCameraTilt(DeltaTime);
}

bool UOvrlParkourComponent::HandleWallrunMovement(bool bIsLeftSide)
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
		const FVector WallForwardDirection = FVector::CrossProduct(WallrunNormal, -CharacterMovement->GetGravityDirection());
		const FVector LaunchVelocity = WallForwardDirection * DefaultMaxWalkSpeed * -WallDirection;

		Character->LaunchCharacter(LaunchVelocity, true, true);
		bHandled = true;
	}

	return bHandled;
}

void UOvrlParkourComponent::HandleWallrunCameraTilt(float DeltaTime)
{
	FRotator TargetRotation = Character->GetControlRotation();

	if (IsWallrunning())
	{
		// Tilt camera depending on which wall the player is on.
		TargetRotation.Roll = MovementType == EParkourMovementType::WallrunLeft ? WallrunCameraTiltAngle : -WallrunCameraTiltAngle;
	}
	else
	{
		TargetRotation.Roll = 0.f;
	}

	// Lerp and apply rotation
	const FRotator FinalRotation = UKismetMathLibrary::RInterpTo(Character->GetControlRotation(), TargetRotation, DeltaTime, 10.f);
	Character->GetController()->SetControlRotation(FinalRotation);
}

void UOvrlParkourComponent::HandleWallrunJump()
{
	if (IsWallrunning())
	{
		const float WallDirection = GetMovementType() == EParkourMovementType::WallrunLeft ? -1.f : 1.f;

		EndWallrun();

		const float AwayVelocity = WallrunJumpVelocity.X;
		const float ForwardVelocity = WallrunJumpVelocity.Y;
		const float UpwardVelocity = WallrunJumpVelocity.Z;

		// Get the wall forward vector as forward vector for the direction
		const FVector ForwardDirection = FVector::CrossProduct(WallrunNormal, -CharacterMovement->GetGravityDirection()) * -WallDirection;

		// Combine all directions together
		const FVector LaunchVelocity = (WallrunNormal * AwayVelocity) + (ForwardDirection * ForwardVelocity) + (-CharacterMovement->GetGravityDirection() * UpwardVelocity);

		//UKismetSystemLibrary::DrawDebugArrow(this, Character->GetActorLocation(), Character->GetActorLocation() + LaunchVelocity, 4.f, FLinearColor::Green, 10.f, 5.f);

		Character->LaunchCharacter(LaunchVelocity, false, true);
	}
}

void UOvrlParkourComponent::ResetWallrun()
{
	if (MovementType == EParkourMovementType::None)
	{
		bCanCheckWallrun = true;
	}
}

void UOvrlParkourComponent::EndWallrun()
{
	if (IsWallrunning())
	{
		bCanCheckWallrun = false;

		MovementType = EParkourMovementType::None;
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UOvrlParkourComponent::ResetWallrun, WallrunResetTime, false);
	}
}

void UOvrlParkourComponent::HandleSliding()
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
		const FVector EndTrace = StartTrace + CharacterMovement->GetGravityDirection() * SlideDistanceCheck;

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

bool UOvrlParkourComponent::ShouldCancelSliding()
{
	if (!IsSliding())
		return false;

	//OvrlLOG("%f", CharacterMovement->GetLastUpdateVelocity().Length());

	return CharacterMovement->GetLastUpdateVelocity().Length() <= 575.f;
}

void UOvrlParkourComponent::CancelSliding()
{
	CharacterMovement->GroundFriction = DefaultGroundFriction;
	CharacterMovement->BrakingDecelerationWalking = DefaultBrakingDecelerationWalking;
	CharacterMovement->MaxWalkSpeedCrouched = DefaultMaxWalkSpeedCrouched;

	MovementType = EParkourMovementType::None;
}

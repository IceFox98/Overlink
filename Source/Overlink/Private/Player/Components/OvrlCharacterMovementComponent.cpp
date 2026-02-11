// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/Components/OvrlCharacterMovementComponent.h"

#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "MotionWarpingComponent.h"
#include "Core/OvrlCameraEventsDefinition.h"

#if ENABLE_DRAW_DEBUG
#include "KismetTraceUtils.h"
#endif

#include "Player/OvrlPlayerCharacter.h"
#include "OvrlUtils.h"

#include "OvrlGameplayTags.h"

UOvrlCharacterMovementComponent::UOvrlCharacterMovementComponent()
{
	GetNavAgentPropertiesRef().bCanCrouch = true;
	GetNavAgentPropertiesRef().bCanJump = true;

	bCanWalkOffLedgesWhenCrouching = true;

	MaxWalkSpeed = 450.f;
	MaxRunSpeed = 800.f;

	// Parkour variables
	WallrunForwardCheckDistance = 75.f;
	WallrunStrafeCheckDistance = 75.f;
	WallrunMinCheckAngle = 0.f;
	WallrunMaxCheckAngle = 57.f;
	WallrunMinCheckVelocityZ = -500.f;
	WallrunStickForce = 100.f;
	WallrunCameraTiltAngle = 15.f;
	WallrunCooldown = .35f;
	WallrunJumpVelocity = FVector(1000.f, 500.f, 800.f);
	VerticalWallrunJumpVelocity = FVector(1000.f, 1500.f, 800.f);
	VerticalWallrunMaxVelocity = 600.f;
	VerticalWallrunVelocityFalloffSpeed = 1.5f;

	SlideDistanceCheck = 200.f;
	SlideForce = 800.f;
	SlideGroundFriction = 0.f;
	SlideBraking = 1000.f;
	SlideMaxWalkSpeedCrouched = 0.f;

	TraversalCheckDistance = FVector2D(100.f, 50.f);
	TraversalLandingPointDistance = 100.f;
	TraversalHandOffset = { 17.f, 5.f };
	MinVaultHeight = 50.f;
	MaxVaultHeight = 120.f;
	MaxVaultOverLength = 200.f;
	MaxLandingPointHeight = MaxVaultHeight + 30.f;
	MinLandingPointHeight = MaxVaultHeight * .5f;

	MinMantleDistance = 90.f;
}

void UOvrlCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	// Use the character movement mode to set the locomotion mode to the right value. This allows you to have a
	// custom set of movement modes but still use the functionality of the default character movement component.

	switch (MovementMode)
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

void UOvrlCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	Character = Cast<AOvrlPlayerCharacter>(GetCharacterOwner());

	// Save original character values due to reset them after applying parkour movement.
	DefaultGravity = GravityScale;
	DefaultMaxWalkSpeed = MaxWalkSpeed;
	DefaultMaxWalkSpeedCrouched = MaxWalkSpeedCrouched;
	DefaultGroundFriction = GroundFriction;
	DefaultBrakingDecelerationWalking = BrakingDecelerationWalking;

	ResetWallrun();
}

void UOvrlCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateGaitStatus();

	if (bHasPlayerJumped)
	{
		HandleTraversals();
	}

	HandleWallrun(DeltaTime);

	if (ShouldCancelSliding())
	{
		// TODO: Replace with Gameplay Ability
		//Character->UnCrouch();
		CancelSliding();
	}
}

void UOvrlCharacterMovementComponent::UpdateGaitStatus()
{
	if (IsSliding() || IsTraversing())
	{
		return;
	}

	const float LastInputVectorLength = GetLastInputVector().Length();
	if (LastInputVectorLength <= KINDA_SMALL_NUMBER)
	{
		if (Gait == OvrlGaitTags::Running)
		{
			StopRunning();
		}

		SetGait(OvrlGaitTags::Idle);
	}
	else if (ShouldRun())
	{
		StartRunning();
	}
	else if (Gait != OvrlGaitTags::Walking)
	{
		StopRunning();
		SetGait(OvrlGaitTags::Walking);
	}
}

bool UOvrlCharacterMovementComponent::ShouldRun() const
{
	return bShouldRun && !Character->IsAiming();
}

void UOvrlCharacterMovementComponent::Crouch(bool bClientSimulation)
{
	if (!HasValidData())
	{
		return;
	}

	if (!bClientSimulation && !CanCrouchInCurrentState())
	{
		return;
	}

	// See if collision is already at desired size.
	if (CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == CrouchedHalfHeight)
	{
		if (!bClientSimulation)
		{
			CharacterOwner->bIsCrouched = true;
		}

		CharacterOwner->OnStartCrouch(0.f, 0.f);
		SetStance(OvrlStanceTags::Crouching);
		return;
	}

	if (bClientSimulation && CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)
	{
		// restore collision size before crouching
		ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight());
		bShrinkProxyCapsule = true;
	}

	// Change collision size to crouching dimensions
	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float OldUnscaledRadius = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
	// Height is not allowed to be smaller than radius.
	const float ClampedCrouchedHalfHeight = FMath::Max3(0.f, OldUnscaledRadius, CrouchedHalfHeight);
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(OldUnscaledRadius, ClampedCrouchedHalfHeight);
	float HalfHeightAdjust = (OldUnscaledHalfHeight - ClampedCrouchedHalfHeight);
	float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	if (!bClientSimulation)
	{
		// Crouching to a larger height? (this is rare)
		if (ClampedCrouchedHalfHeight > OldUnscaledHalfHeight)
		{
			FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(CrouchTrace), false, CharacterOwner);
			FCollisionResponseParams ResponseParam;
			InitCollisionParams(CapsuleParams, ResponseParam);
			const bool bEncroached = GetWorld()->OverlapBlockingTestByChannel(UpdatedComponent->GetComponentLocation() + ScaledHalfHeightAdjust * GetGravityDirection(), GetWorldToGravityTransform(),
				UpdatedComponent->GetCollisionObjectType(), GetPawnCapsuleCollisionShape(SHRINK_None), CapsuleParams, ResponseParam);

			// If encroached, cancel
			if (bEncroached)
			{
				CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(OldUnscaledRadius, OldUnscaledHalfHeight);
				return;
			}
		}

		if (bCrouchMaintainsBaseLocation)
		{
			// Intentionally not using MoveUpdatedComponent, where a horizontal plane constraint would prevent the base of the capsule from staying at the same spot.
			UpdatedComponent->MoveComponent(ScaledHalfHeightAdjust * GetGravityDirection(), UpdatedComponent->GetComponentQuat(), true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
		}

		CharacterOwner->bIsCrouched = true;
	}

	bForceNextFloorCheck = true;

	// OnStartCrouch takes the change from the Default size, not the current one (though they are usually the same).
	const float MeshAdjust = ScaledHalfHeightAdjust;
	ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
	HalfHeightAdjust = (DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - ClampedCrouchedHalfHeight);
	ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	AdjustProxyCapsuleSize();
	CharacterOwner->OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SetStance(OvrlStanceTags::Crouching);

	// Don't smooth this change in mesh position
	if ((bClientSimulation && CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy) || (IsNetMode(NM_ListenServer) && CharacterOwner->GetRemoteRole() == ROLE_AutonomousProxy))
	{
		FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
		if (ClientData)
		{
			ClientData->MeshTranslationOffset -= MeshAdjust * -GetGravityDirection();
			ClientData->OriginalMeshTranslationOffset = ClientData->MeshTranslationOffset;
		}
	}
}

void UOvrlCharacterMovementComponent::UnCrouch(bool bClientSimulation)
{
	if (!HasValidData())
	{
		return;
	}

	ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();

	// See if collision is already at desired size.
	if (CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight())
	{
		if (!bClientSimulation)
		{
			CharacterOwner->bIsCrouched = false;
		}

		CharacterOwner->OnEndCrouch(0.f, 0.f);
		SetStance(OvrlStanceTags::Standing);
		return;
	}

	const float CurrentCrouchedHalfHeight = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float HalfHeightAdjust = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - OldUnscaledHalfHeight;
	const float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
	const FVector PawnLocation = UpdatedComponent->GetComponentLocation();

	// Grow to uncrouched size.
	check(CharacterOwner->GetCapsuleComponent());

	if (!bClientSimulation)
	{
		// Try to stay in place and see if the larger capsule fits. We use a slightly taller capsule to avoid penetration.
		const UWorld* MyWorld = GetWorld();
		const float SweepInflation = UE_KINDA_SMALL_NUMBER * 10.f;
		FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(CrouchTrace), false, CharacterOwner);
		FCollisionResponseParams ResponseParam;
		InitCollisionParams(CapsuleParams, ResponseParam);

		// Compensate for the difference between current capsule size and standing size
		const FCollisionShape StandingCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, -SweepInflation - ScaledHalfHeightAdjust); // Shrink by negative amount, so actually grow it.
		const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();
		bool bEncroached = true;

		if (!bCrouchMaintainsBaseLocation)
		{
			// Expand in place
			bEncroached = MyWorld->OverlapBlockingTestByChannel(PawnLocation, GetWorldToGravityTransform(), CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);

			if (bEncroached)
			{
				// Try adjusting capsule position to see if we can avoid encroachment.
				if (ScaledHalfHeightAdjust > 0.f)
				{
					// Shrink to a short capsule, sweep down to base to find where that would hit something, and then try to stand up from there.
					float PawnRadius, PawnHalfHeight;
					CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);
					const float ShrinkHalfHeight = PawnHalfHeight - PawnRadius;
					const float TraceDist = PawnHalfHeight - ShrinkHalfHeight;
					const FVector Down = TraceDist * GetGravityDirection();

					FHitResult Hit(1.f);
					const FCollisionShape ShortCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, ShrinkHalfHeight);
					const bool bBlockingHit = MyWorld->SweepSingleByChannel(Hit, PawnLocation, PawnLocation + Down, GetWorldToGravityTransform(), CollisionChannel, ShortCapsuleShape, CapsuleParams);
					if (Hit.bStartPenetrating)
					{
						bEncroached = true;
					}
					else
					{
						// Compute where the base of the sweep ended up, and see if we can stand there
						const float DistanceToBase = (Hit.Time * TraceDist) + ShortCapsuleShape.Capsule.HalfHeight;
						const FVector Adjustment = (-DistanceToBase + StandingCapsuleShape.Capsule.HalfHeight + SweepInflation + MIN_FLOOR_DIST / 2.f) * -GetGravityDirection();
						const FVector NewLoc = PawnLocation + Adjustment;
						bEncroached = MyWorld->OverlapBlockingTestByChannel(NewLoc, GetWorldToGravityTransform(), CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
						if (!bEncroached)
						{
							// Intentionally not using MoveUpdatedComponent, where a horizontal plane constraint would prevent the base of the capsule from staying at the same spot.
							UpdatedComponent->MoveComponent(NewLoc - PawnLocation, UpdatedComponent->GetComponentQuat(), false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
						}
					}
				}
			}
		}
		else
		{
			// Expand while keeping base location the same.
			FVector StandingLocation = PawnLocation + (StandingCapsuleShape.GetCapsuleHalfHeight() - CurrentCrouchedHalfHeight) * -GetGravityDirection();
			bEncroached = MyWorld->OverlapBlockingTestByChannel(StandingLocation, GetWorldToGravityTransform(), CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);

			if (bEncroached)
			{
				if (IsMovingOnGround())
				{
					// Something might be just barely overhead, try moving down closer to the floor to avoid it.
					const float MinFloorDist = UE_KINDA_SMALL_NUMBER * 10.f;
					if (CurrentFloor.bBlockingHit && CurrentFloor.FloorDist > MinFloorDist)
					{
						StandingLocation -= (CurrentFloor.FloorDist - MinFloorDist) * -GetGravityDirection();
						bEncroached = MyWorld->OverlapBlockingTestByChannel(StandingLocation, GetWorldToGravityTransform(), CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
					}
				}
			}

			if (!bEncroached)
			{
				// Commit the change in location.
				UpdatedComponent->MoveComponent(StandingLocation - PawnLocation, UpdatedComponent->GetComponentQuat(), false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
				bForceNextFloorCheck = true;
			}
		}

		// If still encroached then abort.
		if (bEncroached)
		{
			return;
		}

		CharacterOwner->bIsCrouched = false;
	}
	else
	{
		bShrinkProxyCapsule = true;
	}

	// Now call SetCapsuleSize() to cause touch/untouch events and actually grow the capsule
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), true);

	const float MeshAdjust = ScaledHalfHeightAdjust;
	AdjustProxyCapsuleSize();
	CharacterOwner->OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SetStance(OvrlStanceTags::Standing);

	// Don't smooth this change in mesh position
	if ((bClientSimulation && CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy) || (IsNetMode(NM_ListenServer) && CharacterOwner->GetRemoteRole() == ROLE_AutonomousProxy))
	{
		FNetworkPredictionData_Client_Character* ClientData = GetPredictionData_Client_Character();
		if (ClientData)
		{
			ClientData->MeshTranslationOffset += MeshAdjust * -GetGravityDirection();
			ClientData->OriginalMeshTranslationOffset = ClientData->MeshTranslationOffset;
		}
	}
}

bool UOvrlCharacterMovementComponent::DoJump(bool bReplayingMoves, float DeltaTime)
{
	if (HandleTraversals())
	{
		return false; // Avoid jump
	}

	OnPlayerJumped();

	// Do the default jump
	return Super::DoJump(bReplayingMoves, DeltaTime);
}

void UOvrlCharacterMovementComponent::OnPlayerJumped()
{
	bShouldSlideOnLanded = false;
	bHasPlayerJumped = true;

	if (IsLateralWallrunning())
	{
		HandleLateralWallrunJump();
	}
	else if (IsVerticalWallrunning() || IsWallClinging())
	{
		HandleVerticalWallrunJump();
	}

	if (LocomotionAction == OvrlLocomotionActionTags::Sliding)
	{
		CancelSliding();
	}
}

void UOvrlCharacterMovementComponent::OnPlayerLanded()
{
	EndWallrun();
	bHasPlayerJumped = false;

	if (bShouldSlideOnLanded)
	{
		Character->Crouch();

		// Is Player still moving?
		if (GetLastUpdateVelocity().Length() > 0.f)
		{
			HandleSliding();
		}

		bShouldSlideOnLanded = false;
	}
}

void UOvrlCharacterMovementComponent::InputStartRun()
{
	bShouldRun = true;
}

void UOvrlCharacterMovementComponent::InputStopRun()
{
	bShouldRun = false;
}

void UOvrlCharacterMovementComponent::StartRunning()
{
	if (Gait != OvrlGaitTags::Running)
	{
		UOvrlUtils::TriggerCameraEvent(this, ECameraFeedbackEvent::StartRun);
	}

	HandleCrouching(false);
	MaxWalkSpeed = MaxRunSpeed;
	SetGait(OvrlGaitTags::Running);
}

void UOvrlCharacterMovementComponent::StopRunning()
{
	MaxWalkSpeed = DefaultMaxWalkSpeed;
	UOvrlUtils::TriggerCameraEvent(this, ECameraFeedbackEvent::StopRun);
	SetGait(OvrlGaitTags::Idle);
}

void UOvrlCharacterMovementComponent::HandleCrouching(bool bInWantsToCrouch)
{
	const bool bIsPlayerGrounded = !IsFalling();

	if (bInWantsToCrouch)
	{
		if (bIsPlayerGrounded)
		{
			Character->Crouch();
		}

		HandleSliding();
	}
	else if (Character->bIsCrouched)
	{
		Character->UnCrouch();

		CancelSliding();
	}
}

void UOvrlCharacterMovementComponent::ResetTraversal()
{
	Character->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SetMovementMode(EMovementMode::MOVE_Falling);
	SetLocomotionAction(FGameplayTag::EmptyTag);
}

bool UOvrlCharacterMovementComponent::HandleTraversals()
{
	if (IsTraversing())
	{
		return false;
	}

	const FTraversalResult TraversalResult = CheckForTraversal();

	//if (false) // Let's overcome the traversal
	if (TraversalResult.bFound) // Let's overcome the traversal
	{
		// Allow the animation's root motion to ignore the gravity.
		SetMovementMode(EMovementMode::MOVE_Flying);

		UpdateHandsIKTransform(TraversalResult);

		switch (TraversalResult.Type)
		{
		case ETraversalType::Vault:
			HandleVault(TraversalResult);
			break;

		case ETraversalType::Mantle:
			HandleMantle(TraversalResult);
			break;

		default:
			break;
		}

		bHasPlayerJumped = false;
		//StopRunning();
		UOvrlUtils::TriggerCameraEvent(this, ECameraFeedbackEvent::StopRun);
		SetGait(OvrlGaitTags::Idle);
		ResetWallrun();
		return true;
	}

	return false;
}

FTraversalResult UOvrlCharacterMovementComponent::CheckForTraversal()
{
	FTraversalResult TraversalResult;

	FVector TraceStart = Character->GetActorLocation();
	FVector TraceEnd = TraceStart + Character->GetActorForwardVector() * TraversalCheckDistance.X;

	const float PlayerCapsuleRadius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
	const float PlayerCapsuleHalfHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	constexpr float TraceCapsuleRadius = 5.f;
	constexpr float TraceCapsuleHalfHeight = 5.f;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Character);
	QueryParams.bFindInitialOverlaps = false;

	// Find the 'smallest' rotation to rotate default gravity vector to be aligned with the custom one
	const FQuat CapsuleRotation = FQuat::FindBetweenNormals(FVector::DownVector, GetGravityDirection());

	// Make first sweep trace to find if there's any obstacle in front of us
	FHitResult ForwardTraversalHit;
	GetWorld()->SweepSingleByChannel(ForwardTraversalHit, TraceStart, TraceEnd, CapsuleRotation, ECC_Visibility, FCollisionShape::MakeCapsule(TraceCapsuleRadius, PlayerCapsuleHalfHeight), QueryParams);

#if ENABLE_DRAW_DEBUG
	const bool bDebugEnabled = UOvrlUtils::ShouldDisplayDebugForActor(GetOwner(), "Ovrl.Traversals");

	if (bDebugEnabled)
		DrawDebugCapsuleTraceSingle(GetWorld(), TraceStart, TraceEnd, TraceCapsuleRadius, PlayerCapsuleHalfHeight, CapsuleRotation.Rotator(), EDrawDebugTrace::ForDuration, ForwardTraversalHit.bBlockingHit, ForwardTraversalHit, FLinearColor::Blue, FLinearColor::Green, 1.f);
#endif

	if (!ForwardTraversalHit.bBlockingHit) // No traversals found
		return TraversalResult;

	const FVector ForwardImpactPoint = ForwardTraversalHit.ImpactPoint;
	const FVector FeetLocation = GetActorFeetLocation();

	// If we found a traversal, we make a downward capsule sweep to find the height of the traversal.
	constexpr float InwardOffset = 10.f;

	// Calculate a vector with opposite direction of the hit normal
	const FVector InwardPosition = ForwardImpactPoint - ForwardTraversalHit.ImpactNormal * InwardOffset;

	// Projection of feet location along the gravity direction. It actually 'filters out' the gravity-relative up component
	const float FeetAlongGravity = FVector::DotProduct(FeetLocation, GetGravityDirection());
	const float InwardAlongGravity = FVector::DotProduct(InwardPosition, GetGravityDirection());

	// Subtract feet Z position to inward one, to get a delta
	TraceEnd = InwardPosition - GetGravityDirection() * (InwardAlongGravity - FeetAlongGravity);
	TraceStart = TraceEnd - GetGravityDirection() * ((PlayerCapsuleHalfHeight * 2.f) + TraversalCheckDistance.Y);

	// Perform downward trace
	FHitResult DownwardTraversalHit;
	GetWorld()->SweepSingleByChannel(DownwardTraversalHit, TraceStart, TraceEnd, CapsuleRotation, ECC_Visibility, FCollisionShape::MakeCapsule(TraceCapsuleRadius, TraceCapsuleHalfHeight), QueryParams);

#if ENABLE_DRAW_DEBUG
	if (bDebugEnabled)
		DrawDebugCapsuleTraceSingle(GetWorld(), TraceStart, TraceEnd, TraceCapsuleRadius, TraceCapsuleHalfHeight, CapsuleRotation.Rotator(), EDrawDebugTrace::ForDuration, DownwardTraversalHit.bBlockingHit, DownwardTraversalHit, FColor::Orange, FLinearColor::Green, 1.f);
#endif

	if (!DownwardTraversalHit.bBlockingHit) // Can't find top of the traversal
		return TraversalResult;

	// This only works on mesh with simple collisions
	if (DownwardTraversalHit.bStartPenetrating)
		return TraversalResult;

	const FVector TraversalTopLocation = DownwardTraversalHit.ImpactPoint;

	// Let's check if player can fit the new location
	QueryParams.bFindInitialOverlaps = true;

	TraceStart = TraversalTopLocation + DownwardTraversalHit.ImpactNormal * (PlayerCapsuleHalfHeight + 10.f);
	TraceEnd = TraceStart;

	FHitResult PlayerHit;
	GetWorld()->SweepSingleByChannel(PlayerHit, TraceStart, TraceEnd, CapsuleRotation, ECC_Visibility, FCollisionShape::MakeCapsule(PlayerCapsuleRadius, PlayerCapsuleHalfHeight), QueryParams);

#if ENABLE_DRAW_DEBUG
	if (bDebugEnabled)
		DrawDebugCapsuleTraceSingle(GetWorld(), TraceStart, TraceEnd, PlayerCapsuleRadius, PlayerCapsuleHalfHeight, CapsuleRotation.Rotator(), EDrawDebugTrace::ForDuration, PlayerHit.bBlockingHit, PlayerHit, FColor::Orange, FLinearColor::Green, 1.f);
#endif

	if (PlayerHit.bBlockingHit) // Player won't fit
		return TraversalResult;

	// Projection of top edge location along the gravity direction. It actually 'filters out' the gravity-relative up component
	const float TopLocationAlongGravity = FVector::DotProduct(TraversalTopLocation, GetGravityDirection());
	const float ForwardPointAlongGravity = FVector::DotProduct(ForwardImpactPoint, GetGravityDirection());
	
	// Subtract top Z position to forward point Z, to get a delta
	const FVector FrontEdgeLocation = ForwardImpactPoint - GetGravityDirection() * (ForwardPointAlongGravity - TopLocationAlongGravity);
	
#if ENABLE_DRAW_DEBUG
	if (bDebugEnabled)
		DrawDebugPoint(GetWorld(), FrontEdgeLocation, 20.f, FColor::Purple, false, 1.f);
#endif

	// Get traversal height along the gravity vector
	const float TraversalHeight = FMath::Abs(FVector::DotProduct(TraversalTopLocation - FeetLocation, GetGravityDirection()));

	// Calculate how far is the front edge
	const float TraversalFrontEdgeDistance = FVector2D::Distance(FVector2D(ForwardTraversalHit.TraceStart), FVector2D(ForwardImpactPoint));

	if (TraversalHeight >= MinVaultHeight && TraversalHeight <= MaxVaultHeight) // Vault
	{
		TraversalResult.bFound = true;
		TraversalResult.FrontEdgeNormal = ForwardTraversalHit.ImpactNormal;
		TraversalResult.FrontEdgeLocation = FrontEdgeLocation;
		TraversalResult.Height = TraversalHeight;
		TraversalResult.Type = ETraversalType::Vault;
		FindLandingPoint(TraversalResult);
	}
	else if (TraversalFrontEdgeDistance <= MinMantleDistance && TraversalHeight > MaxVaultHeight) // Mantle
	{
		TraversalResult.bFound = true;
		TraversalResult.FrontEdgeNormal = ForwardTraversalHit.ImpactNormal;
		TraversalResult.FrontEdgeLocation = FrontEdgeLocation;
		TraversalResult.Height = TraversalHeight;
		TraversalResult.Type = ETraversalType::Mantle;
	}

	return TraversalResult;
}

void UOvrlCharacterMovementComponent::FindLandingPoint(FTraversalResult& OutTraversalResult) const
{
	const FVector DownwardOffset = GetGravityDirection() * 10.f; // Used to don't trace along the upper side of the traversal

	// Perform backward trace to find the back edge of the traversal
	FVector TraceStart = OutTraversalResult.FrontEdgeLocation + DownwardOffset - OutTraversalResult.FrontEdgeNormal * MaxVaultOverLength;
	FVector TraceEnd = OutTraversalResult.FrontEdgeLocation + DownwardOffset;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Character);
	QueryParams.bFindInitialOverlaps = false;

	FHitResult BackEdgeHit;
	GetWorld()->LineTraceSingleByChannel(BackEdgeHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams);

#if ENABLE_DRAW_DEBUG
	const bool bDebugEnabled = UOvrlUtils::ShouldDisplayDebugForActor(GetOwner(), "Ovrl.Traversals");

	if (bDebugEnabled)
		DrawDebugLineTraceSingle(GetWorld(), TraceStart, TraceEnd, EDrawDebugTrace::ForDuration, BackEdgeHit.bBlockingHit, BackEdgeHit, FLinearColor::Red, FLinearColor::Green, 1.f);
#endif

	// Proceed only if the back edge is found and trace didn't start in penetration
	if (BackEdgeHit.bBlockingHit && !BackEdgeHit.bStartPenetrating)
	{
		const float TopLocationAlongGravity = FVector::DotProduct(OutTraversalResult.FrontEdgeLocation, GetGravityDirection());
		const float BackPointAlongGravity = FVector::DotProduct(BackEdgeHit.ImpactPoint, GetGravityDirection());
	
		// Cache back edge location
		OutTraversalResult.BackEdgeLocation = BackEdgeHit.ImpactPoint - GetGravityDirection() * (BackPointAlongGravity - TopLocationAlongGravity);
		
#if ENABLE_DRAW_DEBUG
		if (bDebugEnabled)
			DrawDebugPoint(GetWorld(), OutTraversalResult.BackEdgeLocation, 20.f, FColor::Purple, false, 1.f);
#endif

		// Perform downward trace to find the exact landing point
		TraceStart = OutTraversalResult.BackEdgeLocation - OutTraversalResult.FrontEdgeNormal * TraversalLandingPointDistance;
		TraceEnd = TraceStart + GetGravityDirection() * MaxLandingPointHeight;

		FHitResult LandingPointHit;
		GetWorld()->LineTraceSingleByChannel(LandingPointHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams);

#if ENABLE_DRAW_DEBUG
		if (bDebugEnabled)
			DrawDebugLineTraceSingle(GetWorld(), TraceStart, TraceEnd, EDrawDebugTrace::ForDuration, LandingPointHit.bBlockingHit, LandingPointHit, FLinearColor::Red, FLinearColor::Green, 1.f);
#endif

		if (LandingPointHit.bBlockingHit && LandingPointHit.Distance >= MinLandingPointHeight && LandingPointHit.Distance <= MaxLandingPointHeight)
		{
			// Valid landing point found!
			OutTraversalResult.bHasLandingPoint = true;
			OutTraversalResult.LandingPoint = LandingPointHit.ImpactPoint;

#if ENABLE_DRAW_DEBUG
			if (bDebugEnabled)
				DrawDebugPoint(GetWorld(), OutTraversalResult.LandingPoint, 10.f, FColor::Green, false, 1.f);
#endif
		}
	}
	else // No valid landing point found, maybe the traversal is too long.
	{
		// Set a default back edge location, just to find the mid point for warp data
		OutTraversalResult.BackEdgeLocation = OutTraversalResult.FrontEdgeLocation - OutTraversalResult.FrontEdgeNormal * TraversalLandingPointDistance;
	}
}

void UOvrlCharacterMovementComponent::SetVaultWarpingData(const FTraversalResult& TraversalResult) const
{
	if (UMotionWarpingComponent* MotionWarping = Character->GetMotionWarpingComponent())
	{
		const FRotator WarpRotation = Character->GetActorRotation();

		FVector WarpLocation = FVector::ZeroVector;
		if (TraversalResult.bHasLandingPoint)
		{
			// Find mid point of the traversal
			WarpLocation = (TraversalResult.FrontEdgeLocation + TraversalResult.BackEdgeLocation) * .5f;
		}
		else
		{
			WarpLocation = TraversalResult.FrontEdgeLocation + WarpRotation.Vector() * 50.f;
		}

#if ENABLE_DRAW_DEBUG
		const bool bDebugEnabled = UOvrlUtils::ShouldDisplayDebugForActor(GetOwner(), "Ovrl.Traversals");

		if (bDebugEnabled)
			DrawDebugPoint(GetWorld(), WarpLocation, 10.f, FColor::Green, false, 1.f);
#endif

		FMotionWarpingTarget StartWarpTarget;
		StartWarpTarget.Name = StartTraversalWarpTargetName;
		StartWarpTarget.Location = WarpLocation;
		StartWarpTarget.Rotation = WarpRotation;
		MotionWarping->AddOrUpdateWarpTarget(StartWarpTarget);

		FMotionWarpingTarget EndWarpTarget;
		EndWarpTarget.Name = EndTraversalWarpTargetName;
		EndWarpTarget.Location = TraversalResult.LandingPoint;
		EndWarpTarget.Rotation = WarpRotation;
		MotionWarping->AddOrUpdateWarpTarget(EndWarpTarget);
	}
}

void UOvrlCharacterMovementComponent::SetMantleWarpingData(const FTraversalResult& TraversalResult) const
{
	if (UMotionWarpingComponent* MotionWarping = Character->GetMotionWarpingComponent())
	{
		const FRotator WarpRotation = Character->GetActorRotation();

		FMotionWarpingTarget StartWarpTarget;
		StartWarpTarget.Name = StartTraversalWarpTargetName;
		StartWarpTarget.Rotation = WarpRotation;
		StartWarpTarget.Location = TraversalResult.FrontEdgeLocation;

		MotionWarping->AddOrUpdateWarpTarget(StartWarpTarget);
	}
}

float UOvrlCharacterMovementComponent::FindMontageStartForDeltaZ(UAnimMontage* Montage, double DeltaZ)
{
	if (DeltaZ < 0.f)
	{
		return 0.f;
	}

	// https://landelare.github.io/2022/05/15/climbing-with-root-motion.html
	float Start = 0;
	float End = Montage->GetPlayLength();

	FTransform EndTransform = UOvrlUtils::ExtractRootTransformFromMontage(Montage, End);
	double TargetZ = EndTransform.GetTranslation().Z - DeltaZ;

	// Do a binary search - halve the search interval until we get close enough. 
	while (true)
	{
		float T = (Start + End) * .5f;
		FTransform Transform = UOvrlUtils::ExtractRootTransformFromMontage(Montage, T);
		double ZAtT = Transform.GetTranslation().Z;

		if (T == 0.f)
			return T;

		if (FMath::IsNearlyEqual(ZAtT, TargetZ, 0.5))
			return T;

		if (ZAtT < TargetZ)
			Start = T;
		else
			End = T;
	}
}

void UOvrlCharacterMovementComponent::UpdateHandsIKTransform(const FTraversalResult& TraversalResult)
{
	const FVector FrontEdgeDirection = FVector::CrossProduct(-GetGravityDirection(), TraversalResult.FrontEdgeNormal);

	const FVector HorizontalOffset = FrontEdgeDirection * TraversalHandOffset.X;
	const FVector VerticalOffset = -GetGravityDirection() * TraversalHandOffset.Y;

	RightHandIKLocation = TraversalResult.FrontEdgeLocation - HorizontalOffset + VerticalOffset;
	LeftHandIKLocation = TraversalResult.FrontEdgeLocation + HorizontalOffset + VerticalOffset;
}

void UOvrlCharacterMovementComponent::HandleVault(const FTraversalResult& TraversalResult)
{
	SetVaultWarpingData(TraversalResult);

	if (TraversalResult.bHasLandingPoint)
	{
		// Disable collision just in this case, to avoid jerky movements
		Character->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Character->PlayAnimMontage(VaultOverMontage);
	}
	else
	{
		Character->PlayAnimMontage(VaultClimbUpMontage);
	}

	//Character->Crouch();
	SetLocomotionAction(OvrlLocomotionActionTags::Vaulting);
}

void UOvrlCharacterMovementComponent::HandleMantle(const FTraversalResult& TraversalResult)
{
	SetMantleWarpingData(TraversalResult);

	const float StartTime = FindMontageStartForDeltaZ(MantleMontage, TraversalResult.Height);

	Character->PlayAnimMontage(MantleMontage, StartTime);
	SetLocomotionAction(OvrlLocomotionActionTags::Mantling);
}

bool UOvrlCharacterMovementComponent::ShouldHandleWallrun() const
{
	const FVector LastVelocity = GetLastUpdateVelocity();
	return IsFalling() && bHasPlayerJumped && !bIsWallrunInCooldown && LastVelocity.Z > WallrunMinCheckVelocityZ;
}

void UOvrlCharacterMovementComponent::HandleWallrun(float DeltaTime)
{
	if (ShouldHandleWallrun())
	{
		//if (IsWallrunning())
		//{
		//	if (HandleTraversals()) // Any traversal while wallrunning?
		//	{
		//		// Reset wallrun and perform the traversal
		//		ResetWallrun();
		//		return;
		//	}
		//}

		const bool bIsPlayerMoving = GetLastInputVector().Length() > 0.f;

		//if (bIsPlayerNotGrounded/* && bIsPlayerMovingForward*/)
		{
			bool bHandled = HandleVerticalWallrun(DeltaTime);
			if (!bHandled /*&& bIsPlayerMoving*/)
			{
				const bool bHasVelocity = GetLastUpdateVelocity().Size2D() > 0.f; // @TODO: Adapt to gravity direction?
				if (!bHasVelocity)
				{
					EndWallrun();
				}

				bHandled = HandleLateralWallrun(DeltaTime, true);
				if (!bHandled)
				{
					HandleLateralWallrun(DeltaTime, false);
				}
			}
		}
	}

	// The tilting is now managed in the PlayerController class
	//HandleWallrunCameraTilt(DeltaTime);
}

bool UOvrlCharacterMovementComponent::HandleVerticalWallrun(float DeltaTime)
{
	//if (IsLateralWallrunning())
	//{
	//	return false;
	//}

	const FVector ForwardVector = Character->GetActorForwardVector() * WallrunForwardCheckDistance;

	const FVector StartTrace = Character->GetActorLocation();
	const FVector EndTrace = Character->GetActorLocation() + ForwardVector;
	EDrawDebugTrace::Type DebugType = EDrawDebugTrace::None;

#if ENABLE_DRAW_DEBUG
	if (UOvrlUtils::ShouldDisplayDebugForActor(GetOwner(), "Ovrl.Wallrun"))
		DebugType = EDrawDebugTrace::ForOneFrame;
#endif

	FHitResult OutHit;
	UKismetSystemLibrary::LineTraceSingle(this, StartTrace, EndTrace, ETraceTypeQuery::TraceTypeQuery1, false, {}, DebugType, OutHit, true, FLinearColor::Blue);

	if (OutHit.bBlockingHit)
	{
		const bool bShouldPerformWallrun = FVector::DotProduct(Character->GetActorForwardVector(), -OutHit.ImpactNormal) > .85f;

		if (bShouldPerformWallrun)
		{
			WallrunNormal = OutHit.ImpactNormal;

			if (!IsWallClinging())
			{
				SetLocomotionAction(OvrlLocomotionActionTags::WallrunningVertical);
			}
		}
	}

	if (IsVerticalWallrunning() || IsWallClinging())
	{
		// Push the player up along the wall, easing out the vertical velocity
		float VelocityZ = FMath::InterpEaseOut(0.f, VerticalWallrunMaxVelocity, VerticalWallrunAlpha, 3.f);
		VerticalWallrunAlpha = FMath::Clamp(VerticalWallrunAlpha - VerticalWallrunVelocityFalloffSpeed * DeltaTime, 0.f, 1.f);

		if (VelocityZ <= 0.f)
		{
			SetLocomotionAction(OvrlLocomotionActionTags::WallClinging);
		}

		// The force used to keep the player sticked to the wall
		const FVector StickVelocity = -WallrunNormal * WallrunStickForce;

		const FVector LaunchVelocity = -GetGravityDirection() * VelocityZ;
		Character->LaunchCharacter(LaunchVelocity + StickVelocity, true, true);

		return true;
	}

	return false;
}

bool UOvrlCharacterMovementComponent::HandleLateralWallrun(float DeltaTime, bool bIsLeftSide)
{
	const FGameplayTag WallrunTag = bIsLeftSide ? OvrlLocomotionActionTags::WallrunningLeft : OvrlLocomotionActionTags::WallrunningRight;

	// Avoid trace for a wall that we're not wallrunning
	if (IsLateralWallrunning() && LocomotionAction != WallrunTag)
	{
		return false;
	}

	const float WallDirection = bIsLeftSide ? -1.f : 1.f;
	const FVector WallrunCheckVector = (Character->GetActorRightVector() * WallrunStrafeCheckDistance * WallDirection) +
	                                   Character->GetActorForwardVector() * WallrunForwardCheckDistance;

	const FVector StartTrace = Character->GetActorLocation();
	const FVector EndTrace = Character->GetActorLocation() + WallrunCheckVector;
	EDrawDebugTrace::Type DebugType = EDrawDebugTrace::None;

#if ENABLE_DRAW_DEBUG
	if (UOvrlUtils::ShouldDisplayDebugForActor(GetOwner(), "Ovrl.Wallrun"))
		DebugType = EDrawDebugTrace::ForOneFrame;
#endif

	FHitResult OutHit;
	UKismetSystemLibrary::LineTraceSingle(this, StartTrace, EndTrace, ETraceTypeQuery::TraceTypeQuery1, false, {}, DebugType, OutHit, true);

	// We get the normal only when we detect a wall, otherwise we use the cached one.
	if (OutHit.bBlockingHit)
	{
		WallrunNormal = OutHit.Normal;

		if (!IsLateralWallrunning())
		{
			const float MinCheckAngle = FMath::Cos(FMath::DegreesToRadians(90.f - WallrunMinCheckAngle));
			const float MaxCheckAngle = FMath::Cos(FMath::DegreesToRadians(90.f - WallrunMaxCheckAngle));

			const double CheckAngle = FVector::DotProduct(-WallrunNormal, Character->GetActorForwardVector());

			if (CheckAngle < MinCheckAngle || CheckAngle > MaxCheckAngle)
			{
				//EndWallrun();
				return false;
			}
		}

		const FGameplayTag TargetGameplayTag = bIsLeftSide ? OvrlLocomotionActionTags::WallrunningLeft : OvrlLocomotionActionTags::WallrunningRight;
		SetLocomotionAction(TargetGameplayTag);
	}
	else if (IsLateralWallrunning()) // Perform validation trace only if we're actually wallrunning and we're not hitting any walls
	{
		// Let's do another trace directly towards the wall, to check if the player is still sticked to the wall.
		// Using the first trace is unsafe, since it's handled by the player forward vector.
		const FVector ValidationStartTrace = Character->GetActorLocation();
		const FVector ValidationEndTrace = Character->GetActorLocation() - WallrunNormal * 100.f;

		FHitResult ValidationHit;
		UKismetSystemLibrary::LineTraceSingle(this, ValidationStartTrace, ValidationEndTrace, ETraceTypeQuery::TraceTypeQuery1, false, {}, DebugType, ValidationHit, true);

		if (!ValidationHit.bBlockingHit)
		{
			EndWallrun();
			return false;
		}
	}

	// Execute only the wallrun we're actually performing
	if (IsLateralWallrunning())
	{
		// Let's give the player an upward push when it starts wallrunning, that will decrease over time.
		constexpr float CurveExponent = .5f; // How fast the velocity should decrease at the end
		const FVector UpVelocity = -GetGravityDirection() * FMath::InterpEaseOut(0.f, VerticalWallrunMaxVelocity, LateralWallrunAlpha, CurveExponent);
		LateralWallrunAlpha = FMath::Clamp(LateralWallrunAlpha - DeltaTime, 0.f, 1.f);

		// Force the player to stick to the wall
		const FVector LateralVelocity = -WallrunNormal * WallrunStickForce;

		// Returns the direction of where the player should be launched. It follows the wall surface.
		const FVector WallForwardDirection = FVector::CrossProduct(WallrunNormal, -GetGravityDirection());
		const FVector LaunchVelocity = WallForwardDirection * MaxWalkSpeed * -WallDirection;

		Character->LaunchCharacter(LaunchVelocity + UpVelocity + LateralVelocity, true, true);

		return true;
	}

	return false;
}

void UOvrlCharacterMovementComponent::HandleWallrunCameraTilt(float DeltaTime)
{
	//FRotator TargetRotation = Character->GetControlRotation();

	//if (IsWallrunning())
	//{
	//	// Tilt camera depending on which wall the player is on.
	//	TargetRotation.Roll = LocomotionAction == OvrlLocomotionActionTags::WallrunningLeft ? WallrunCameraTiltAngle : -WallrunCameraTiltAngle;
	//}
	//else
	//{
	//	TargetRotation.Roll = 0.f;
	//}

	//// Lerp and apply rotation
	//const FRotator FinalRotation = UKismetMathLibrary::RInterpTo(Character->GetControlRotation(), TargetRotation, DeltaTime, 10.f);
	//Character->GetController()->SetControlRotation(FinalRotation);
}

void UOvrlCharacterMovementComponent::HandleLateralWallrunJump()
{
	const float WallDirection = LocomotionAction == OvrlLocomotionActionTags::WallrunningLeft ? -1.f : 1.f;
	const float AwayVelocity = WallrunJumpVelocity.X;
	const float ForwardVelocity = WallrunJumpVelocity.Y;
	const float UpwardVelocity = WallrunJumpVelocity.Z;

	// Get the wall forward vector as forward vector for the direction
	const FVector ForwardDirection = FVector::CrossProduct(WallrunNormal, -GetGravityDirection()) * -WallDirection;

	// Combine all directions together
	const FVector LaunchVelocity = (WallrunNormal * AwayVelocity) + (ForwardDirection * ForwardVelocity) + (-GetGravityDirection() * UpwardVelocity);

	//UKismetSystemLibrary::DrawDebugArrow(this, Character->GetActorLocation(), Character->GetActorLocation() + LaunchVelocity, 4.f, FLinearColor::Green, 10.f, 5.f);

	EndWallrun();
	Character->LaunchCharacter(LaunchVelocity, false, true);
}

void UOvrlCharacterMovementComponent::HandleVerticalWallrunJump()
{
	const float AwayVelocity = VerticalWallrunJumpVelocity.X;
	const float ForwardVelocity = VerticalWallrunJumpVelocity.Y;
	const float UpwardVelocity = VerticalWallrunJumpVelocity.Z;

	FVector LaunchVelocity = FVector::ZeroVector;

	// How much the player input is aligned with the wall normal
	const float InputAlignedDotValue = FVector::DotProduct(GetLastInputVector().GetSafeNormal(), WallrunNormal);

	// How muche the player pointing direction is aligned with the wall normal
	const float PlayerAlignedDotValue = FVector::DotProduct(Character->GetActorForwardVector(), WallrunNormal);

	const bool bHasNoInput = FMath::IsNearlyZero(GetLastInputVector().Length());

	if (bHasNoInput)
	{
		if (PlayerAlignedDotValue < 0.f) // Facing the wall, no input --> back jump
		{
			LaunchVelocity = (WallrunNormal * AwayVelocity) + (-Character->GetActorForwardVector() * ForwardVelocity) + (-GetGravityDirection() * UpwardVelocity);
		}
		else // Facing out of the wall, no input --> jump out of the wall
		{
			LaunchVelocity = (WallrunNormal * AwayVelocity) + (-GetGravityDirection() * UpwardVelocity);
		}
	}
	else
	{
		if (PlayerAlignedDotValue < 0.f) // Facing the wall, has input --> up jump
		{
			LaunchVelocity = /*(Character->GetActorForwardVector() * ForwardVelocity) + */(-GetGravityDirection() * UpwardVelocity);
		}
		else // Facing out of the wall, has input --> jump toward the input direction
		{
			LaunchVelocity = (Character->GetActorForwardVector() * ForwardVelocity) + (-GetGravityDirection() * UpwardVelocity);
		}
	}

	EndWallrun();
	Character->LaunchCharacter(LaunchVelocity, true, true);
}

void UOvrlCharacterMovementComponent::ResetWallrun()
{
	//if (LocomotionAction == FGameplayTag::EmptyTag)
	{
		bIsWallrunInCooldown = false;
		VerticalWallrunAlpha = 1.f;
		LateralWallrunAlpha = 1.f;
	}
}

void UOvrlCharacterMovementComponent::EndWallrun()
{
	if (IsWallrunning() || IsWallClinging())
	{
		bIsWallrunInCooldown = true;
		SetLocomotionAction(FGameplayTag::EmptyTag);

		// Allow player to wallrun again, only after a specific amount of time
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UOvrlCharacterMovementComponent::ResetWallrun, WallrunCooldown, false);
	}
}

void UOvrlCharacterMovementComponent::HandleSliding()
{
	if (IsSliding())
		return;

	const bool bIsPlayerGrounded = !IsFalling();
	const bool bIsPlayerMovingForward = IsMovingForward(60.f);
	const bool bIsPlayerRunning = GetGait() == OvrlGaitTags::Running;

	bShouldSlideOnLanded = !bIsPlayerGrounded;

	if (bIsPlayerGrounded && bIsPlayerMovingForward && bIsPlayerRunning)
	{
		const FVector StartTrace = Character->GetActorLocation();
		const FVector EndTrace = StartTrace + GetGravityDirection() * SlideDistanceCheck;
		EDrawDebugTrace::Type DebugType = EDrawDebugTrace::None;

#if ENABLE_DRAW_DEBUG
		if (UOvrlUtils::ShouldDisplayDebugForActor(GetOwner(), "Ovrl.Traversals"))
			DebugType = EDrawDebugTrace::ForOneFrame;
#endif

		// Trace a down vector to check if sliding is possible
		FHitResult OutHit;
		UKismetSystemLibrary::LineTraceSingle(this, StartTrace, EndTrace, ETraceTypeQuery::TraceTypeQuery1, false, {}, DebugType, OutHit, true);

		if (OutHit.bBlockingHit)
		{
			// Change movement params to simulate sliding
			GroundFriction = SlideGroundFriction;
			BrakingDecelerationWalking = SlideBraking;
			MaxWalkSpeedCrouched = SlideMaxWalkSpeedCrouched;

			// Cross with floor normal to get the direction of where to launche the character
			const FVector FloorSlopeDirection = FVector::CrossProduct(Character->GetActorRightVector(), OutHit.Normal);
			AddImpulse(FloorSlopeDirection * SlideForce, true);

			StopRunning();
			SetLocomotionAction(OvrlLocomotionActionTags::Sliding);
		}
	}
}

bool UOvrlCharacterMovementComponent::ShouldCancelSliding()
{
	if (!IsSliding())
	{
		return false;
	}

	//OvrlLOG("%f", GetLastUpdateVelocity().Length());

	return GetLastUpdateVelocity().Length() <= 575.f;
	//return false;
}

void UOvrlCharacterMovementComponent::CancelSliding()
{
	GroundFriction = DefaultGroundFriction;
	BrakingDecelerationWalking = DefaultBrakingDecelerationWalking;
	MaxWalkSpeedCrouched = DefaultMaxWalkSpeedCrouched;

	SetLocomotionAction(FGameplayTag::EmptyTag);
}

double UOvrlCharacterMovementComponent::GetDesiredCameraRoll()
{
	double TargetRoll = 0;

	if (IsLateralWallrunning())
	{
		TargetRoll = LocomotionAction == OvrlLocomotionActionTags::WallrunningLeft ? WallrunCameraTiltAngle : -WallrunCameraTiltAngle;
	}

	return TargetRoll;
}

void UOvrlCharacterMovementComponent::ApplyCameraPitchLimits(float& ViewPitchMin, float& ViewPitchMax)
{
	if (IsVerticalWallrunning())
	{
		ViewPitchMin = VerticalWallrunCameraLimits.ViewPitchMin;
		ViewPitchMax = VerticalWallrunCameraLimits.ViewPitchMax;
	}
}

void UOvrlCharacterMovementComponent::ApplyCameraYawLimits(float& ViewYawMin, float& ViewYawMax)
{
	if (IsVerticalWallrunning() || IsWallClinging())
	{
		ViewYawMin = VerticalWallrunCameraLimits.ViewYawMin;
		ViewYawMax = VerticalWallrunCameraLimits.ViewYawMax;
	}
	else if (IsLateralWallrunning())
	{
		const bool bIsRightWall = LocomotionAction == OvrlLocomotionActionTags::WallrunningLeft;
		const float WallDirection = bIsRightWall ? 1.f : -1.f;
		const FVector WallForwardDirection = FVector::CrossProduct(WallrunNormal, -GetGravityDirection()) * WallDirection;
		const float WallYaw = WallForwardDirection.Rotation().Yaw;

		//OVRL_LOG("%f", WallYaw);

		ViewYawMin = WallYaw + WallrunCameraLimits.ViewYawMin * -WallDirection;
		ViewYawMax = WallYaw + WallrunCameraLimits.ViewYawMax * WallDirection;

		if (WallYaw <= 0.f && WallYaw > -180.f)
		{
			float Temp = ViewYawMin;
			ViewYawMin = ViewYawMax;
			ViewYawMax = Temp;
		}
	}
}

FVector UOvrlCharacterMovementComponent::GetRelativeLastUpdateVelocity()
{
	return Character ? Character->GetActorTransform().InverseTransformVector(GetLastUpdateVelocity()) : FVector::ZeroVector;
}

// Returns true if the angle between forward vector and input vector is between an imaginary cone, with AngleFromForwardVector as angle.
bool UOvrlCharacterMovementComponent::IsMovingForward(float AngleFromForwardVector/* = 90.f*/)
{
	// Normalize between 0-1. 90° -> 0 (perpendicular). 0° -> 1 (perfectly aligned)
	const float NormalizedAngle = FMath::Cos(FMath::DegreesToRadians(AngleFromForwardVector));
	// Since the input vector is not relative to the player, we can easily check if the we're going forward using dot product
	return FVector::DotProduct(GetLastInputVector(), Character->GetActorForwardVector()) >= NormalizedAngle;
}

void UOvrlCharacterMovementComponent::SetLocomotionAction(const FGameplayTag& NewLocomotionAction)
{
	if (LocomotionAction == NewLocomotionAction)
		return;

	LocomotionAction = NewLocomotionAction;
}

void UOvrlCharacterMovementComponent::SetLocomotionMode(const FGameplayTag& NewLocomotionMode)
{
	if (LocomotionMode == NewLocomotionMode)
		return;

	LocomotionMode = NewLocomotionMode;
}

void UOvrlCharacterMovementComponent::SetStance(const FGameplayTag& NewStance)
{
	if (Stance == NewStance)
		return;

	const FGameplayTag OldStance = Stance;
	Stance = NewStance;

	OnStanceChanged.Broadcast(OldStance, NewStance);
}

void UOvrlCharacterMovementComponent::SetGait(const FGameplayTag& NewGait)
{
	if (Gait == NewGait)
		return;

	const FGameplayTag OldGait = Gait;
	Gait = NewGait;

	OnGaitChanged.Broadcast(OldGait, NewGait);
}

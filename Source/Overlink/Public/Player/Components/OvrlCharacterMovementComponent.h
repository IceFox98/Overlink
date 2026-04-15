// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "OvrlGameplayTags.h"

#include "OvrlCharacterMovementComponent.generated.h"

class AOvrlPlayerCharacter;

enum class ETraversalType : uint8
{
	None,
	Vault,
	Mantle
};

enum class EWallrunType : uint8
{
	Vertical,
	Left,
	Right
};

struct FTraversalResult
{
	// Set to true if any traversal has been detected.
	bool bFound = false;

	// If true, there is a valid landing point beyond the traversal.
	// The validation of the landing point is determined by MinLandingPointHeight and MaxLandingPointHeight: if the landing point stands between 
	// this range, then it's valid, otherwise the character should either mantle or vault and fall.
	bool bHasLandingPoint = false;

	// The location of the nearest edge in front of us
	FVector FrontEdgeLocation;

	// The location of the nearest edge in front of us
	FVector BackEdgeLocation;

	// The normal of the front "wall" we have in front of us.
	FVector FrontEdgeNormal;

	// Where the player should land and end its animation
	FVector LandingPoint;

	// The height of the traversal, considering the player's feet position as starting point 
	float Height;

	ETraversalType Type = ETraversalType::None;
};

USTRUCT()
struct FCameraLimits
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float ViewPitchMin = -89.9f;

	UPROPERTY(EditAnywhere)
	float ViewPitchMax = 89.9f;

	UPROPERTY(EditAnywhere)
	float ViewYawMin = 0.f;

	UPROPERTY(EditAnywhere)
	float ViewYawMax = 90.f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStanceChanged, const FGameplayTag&, OldStance, const FGameplayTag&, NewStance);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGaitChanged, const FGameplayTag&, OldGait, const FGameplayTag&, NewGait);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLocomotionActionChanged, const FGameplayTag&, OldLocomotionAction, const FGameplayTag&, NewLocomotionAction);

/**
 *
 */
UCLASS()
class OVERLINK_API UOvrlCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UOvrlCharacterMovementComponent();

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void Crouch(bool bClientSimulation = false) override;
	virtual void UnCrouch(bool bClientSimulation = false) override;
	virtual bool DoJump(bool bReplayingMoves, float DeltaTime) override;

public:
	// ------ LOCOMOTION ------
	
	// Returns the last update velocity, but relative to the player
	FVector GetRelativeLastUpdateVelocity() const;

	bool IsMovingForward(float AngleFromForwardVector = 90.f);
	
	UFUNCTION(BlueprintCallable, Category = "Ovrl Character Movement Component|Traversal")
	void OnPlayerLanded();

	void InputStartRun();
	void InputStopRun();

	void HandleCrouching(bool bInWantsToCrouch);

	FORCEINLINE bool IsRunning() const { return Gait == OvrlGaitTags::Running; };
	FORCEINLINE const FGameplayTag& GetLocomotionAction() const { return LocomotionAction; }
	FORCEINLINE const FGameplayTag& GetLocomotionMode() const { return LocomotionMode; }
	FORCEINLINE const FGameplayTag& GetStance() const { return Stance; }
	FORCEINLINE const FGameplayTag& GetGait() const { return Gait; }

	void SetLocomotionAction(const FGameplayTag& NewLocomotionAction);
	void SetLocomotionMode(const FGameplayTag& NewLocomotionMode);
	void SetStance(const FGameplayTag& NewStance);
	void SetGait(const FGameplayTag& NewGait);
	
	// ------ GENERAL ------
	
	void OnPlayerJumped();
	double GetDesiredCameraRoll() const;
	void ApplyCameraPitchLimits(float& ViewPitchMin, float& ViewPitchMax);
	void ApplyCameraYawLimits(float& ViewYawMin, float& ViewYawMax);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ovrl Character Movement Component")
	FORCEINLINE FVector GetGroundNormal() const { return GroundNormal; };

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ovrl Character Movement Component")
	FORCEINLINE FVector GetWallrunNormal() const { return WallrunNormal; };

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ovrl Character Movement Component")
	FORCEINLINE float GetLastValidWallDirection() const { return LastWallDirection; };

	FORCEINLINE FVector GetRightHandIKLocation() const { return RightHandIKLocation; };
	FORCEINLINE FVector GetLeftHandIKLocation() const { return LeftHandIKLocation; };
	
	// ------ TRAVERSALS ------

	UFUNCTION(BlueprintCallable, Category = "Ovrl Character Movement Component|Traversal")
	void ResetTraversal();

	UFUNCTION(BlueprintCallable, Category = "Ovrl Character Movement Component|Traversal")
	FORCEINLINE bool IsTraversing() const
	{
		return LocomotionAction == OvrlLocomotionActionTags::Mantling || LocomotionAction == OvrlLocomotionActionTags::Vaulting;
	};
	FORCEINLINE bool IsWallrunning() const { return IsLateralWallrunning() || IsVerticalWallrunning(); };
	FORCEINLINE bool IsLateralWallrunning() const
	{
		return LocomotionAction == OvrlLocomotionActionTags::WallrunningLeft || LocomotionAction == OvrlLocomotionActionTags::WallrunningRight;
	};
	FORCEINLINE bool IsVerticalWallrunning() const { return LocomotionAction == OvrlLocomotionActionTags::WallrunningVertical; };
	FORCEINLINE bool IsWallClinging() const { return LocomotionAction == OvrlLocomotionActionTags::WallClinging; };
	FORCEINLINE bool IsSliding() const { return LocomotionAction == OvrlLocomotionActionTags::Sliding; };

protected:
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void BeginPlay() override;

	void StartRunning();
	void StopRunning();

private:
	void ComputeGroundNormal();
	void UpdateGaitStatus();
	bool ShouldRun() const;

	// ------------------------
	// ------ TRAVERSALS ------
	// ------------------------

	FTraversalResult CheckForTraversal();
	void SetVaultWarpingData(const FTraversalResult& TraversalResult) const;
	void SetMantleWarpingData(const FTraversalResult& TraversalResult) const;
	float FindMontageStartForDeltaZ(UAnimMontage* Montage, double DeltaZ);
	void UpdateHandsIKTransform(const FTraversalResult& TraversalResult);

	void FindLandingPoint(FTraversalResult& OutTraversalResult) const;
	bool HandleTraversals();

	// ------ VAULT ------
	void HandleVault(const FTraversalResult& TraversalResult);

	// ------ MANTLE ------
	void HandleMantle(const FTraversalResult& TraversalResult);

	// ------ WALLRUN ------

	bool ShouldHandleWallrun() const;

	void HandleWallrun(float DeltaTime);

	bool HandleVerticalWallrun(float DeltaTime);
	bool HandleLateralWallrun(float DeltaTime, bool bIsLeftSide);
	void HandleLateralWallrunJump();
	void HandleVerticalWallrunJump();
	void JumpFromLateralWallrun(const FVector& LaunchVelocity);

	void ResetWallrun();
	void EndWallrun();

	// ------ SLIDE ------

	void HandleSliding();
	bool ShouldCancelSliding() const;
	void CancelSliding();

public:
	UPROPERTY(BlueprintAssignable)
	FOnStanceChanged OnStanceChanged;

	UPROPERTY(BlueprintAssignable)
	FOnGaitChanged OnGaitChanged;

	UPROPERTY(BlueprintAssignable)
	FOnLocomotionActionChanged OnLocomotionActionChanged;

	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component")
	float MaxRunSpeed;

	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component")
	float GroundNormalCheckDistance;

	// -----------------------------
	// ------ PARKOUR SECTION ------
	// -----------------------------

	// ------ TRAVERSAL VARS ------

	// The distance (forward and upward) to check if there are any traversals in front of us.
	// The forward check starts from the center of the player, whereas the upward check starts from the top of the player capsule
	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Traversal")
	FVector2D TraversalCheckDistance;

	// The offset between the front edge and the player right hand.
	// The value will be mirrored for the left hand.
	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Traversal")
	FVector2D TraversalHandOffset;

	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Traversal|Warping")
	FName StartTraversalWarpTargetName;

	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Traversal|Warping")
	FName EndTraversalWarpTargetName;

	// The minimum height of the wall for which vault can be performed
	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Traversal|Vault")
	float MinVaultHeight;

	// The maximum height of the wall for which vault can be performed
	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Traversal|Vault")
	float MaxVaultHeight;

	// The maximum traversal length for which a vault over animation can be performed
	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Traversal|Vault")
	float MaxVaultOverLength;

	// The distance between the back edge traversal and where the player should land
	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Traversal|Vault")
	float TraversalLandingPointDistance;

	// The minimum distance from back edge to floor level, for the landing point to be considered valid
	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Traversal|Vault")
	float MinLandingPointHeight;

	// The maximum distance from back edge to floor level, for the landing point to be considered valid
	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Traversal|Vault")
	float MaxLandingPointHeight;

	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Traversal|Vault")
	TObjectPtr<UAnimMontage> VaultOverMontage;

	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Traversal|Vault")
	TObjectPtr<UAnimMontage> VaultClimbUpMontage;

	// The minimum distance between the player and the front traversal, for the mantle to be performed
	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Traversal|Mantle")
	float MinMantleDistance;

	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Traversal|Mantle")
	TObjectPtr<UAnimMontage> MantleMontage;

	// ------ WALLRUN VARS ------

	// If true, the system will check for traversals during any wallrun.
	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Wallrun")
	bool bAllowTraversalWhenWallrunning;

	// Cooldown time before any wallrun can be performed again.
	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Wallrun")
	float WallrunCooldown;

	// The forward distance, from the player center, from where the wallrun check will end
	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Wallrun")
	float WallrunForwardCheckDistance;
	
	// The minimum Z velocity the player must have to start lateral wallrun.
	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Wallrun")
	float WallrunMinCheckVelocityZ;

	// The lateral distance, from the player center, from where the wallrun check will end
	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Wallrun|Lateral")
	float WallrunStrafeCheckDistance;

	// The maximum outward angle from the wall’s forward vector within which wallrun is allowed.
	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Wallrun|Lateral", meta=(ClampMin = 0))
	float WallrunMaxOuterCheckAngle;

	// The maximum inward angle from the wall’s forward vector within which wallrun is allowed.
	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Wallrun|Lateral", meta=(ClampMin = 0))
	float WallrunMaxInnerCheckAngle;

	// The force applied to keep the player sticked to the wall during lateral wallrun.
	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Wallrun|Lateral", meta=(ClampMin = 0))
	float WallrunStickForce;

	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Wallrun|Lateral")
	FCameraLimits WallrunCameraLimits;

	// The roll applied to the camera during wallrun.
	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Wallrun|Lateral")
	float WallrunCameraTiltAngle;

	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Wallrun|Lateral")
	FVector WallrunJumpVelocity;

	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Wallrun|Vertical")
	FCameraLimits VerticalWallrunCameraLimits;

	// The minimum angle (in degrees) the player’s forward vector and the wall, to perform a lateral wallrun
	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Wallrun|Vertical")
	float VerticalWallrunCheckAngle;

	// The vertical velocity when the player start to wallrun vertically. It will decrease over time.
	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Wallrun|Vertical")
	float VerticalWallrunMaxVelocity;

	// How fast the vertical velocity of the wallrun decrease.
	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Wallrun|Vertical")
	float VerticalWallrunVelocityFalloffSpeed;

	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Wallrun|Vertical")
	FVector VerticalWallrunJumpVelocity;

	// ------ SLIDING VARS ------

	// Vector that will be added to the player position, used to get the slope of the floor.
	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Sliding")
	float SlideDistanceCheck;

	// If the player velocity vector length is lower than this value, the slide will be canceled.
	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Sliding")
	float SlideCancelThreshold;

	// The force of the impulse applied to the player when starts sliding.
	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Sliding")
	float SlideForce;

	// The ground friction applied to the movement component (see GroundFriction) when player is sliding.
	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Sliding")
	float SlideGroundFriction;

	// The braking deceleration applied to the movement component (see BrakingDecelerationWalking) when player is sliding.
	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Sliding")
	float SlideBraking;

	// The max movement speed when player is sliding (see MaxWalkSpeedCrouched of movement component)
	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Sliding")
	float SlideMaxWalkSpeedCrouched;

	// ------ LOCOMOTION ------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Character Movement Component|States", Transient)
	FGameplayTag LocomotionMode = OvrlLocomotionModeTags::Grounded;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Character Movement Component|States", Transient)
	FGameplayTag Stance = OvrlStanceTags::Standing;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Character Movement Component|States", Transient)
	FGameplayTag Gait = OvrlGaitTags::Idle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Character Movement Component|States", Transient)
	FGameplayTag LocomotionAction = FGameplayTag::EmptyTag;

private:
	// ------ DEFAULT VALUES ------

	UPROPERTY()
	TObjectPtr<AOvrlPlayerCharacter> Character;

	float DefaultGravity;
	float DefaultMaxWalkSpeed;
	float DefaultMaxWalkSpeedCrouched;
	float DefaultGroundFriction;
	float DefaultBrakingDecelerationWalking;

	bool bShouldRun;
	FVector GroundNormal;

	// ------ HAND IK ------
	FVector RightHandIKLocation;
	FVector LeftHandIKLocation;

	// ------ WALLRUN VARS ------

	bool bHasPlayerJumped;
	bool bIsWallrunInCooldown;
	FVector WallrunNormal;

	float VerticalWallrunAlpha;
	float LateralWallrunAlpha;

	float LastWallDirection;

	// ------ SLIDING VARS ------

	bool bShouldSlideOnLanded;
};

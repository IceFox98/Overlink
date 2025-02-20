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

struct FTraversalResult
{
	// Set to true if any traversal has detected.
	bool bFound = false;

	ETraversalType Type = ETraversalType::None;
};

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

	// ------ PARKOUR ------

	UFUNCTION(BlueprintCallable, Category = "Ovrl Character Movement Component")
		void OnPlayerJumped();

	UFUNCTION(BlueprintCallable, Category = "Ovrl Character Movement Component")
		void OnPlayerLanded();

	UFUNCTION(BlueprintCallable, Category = "Ovrl Character Movement Component")
		void HandleCrouching(bool bInWantsToCrouch);

	FORCEINLINE bool IsWallrunning() const { return LocomotionAction == OvrlLocomotionActionTags::WallrunningLeft || LocomotionAction == OvrlLocomotionActionTags::WallrunningRight; };

	FORCEINLINE bool IsSliding() const { return LocomotionAction == OvrlLocomotionActionTags::Sliding; };

	// ------ LOCOMOTION ------

	FORCEINLINE const FGameplayTag& GetLocomotionAction() const { return LocomotionAction; }
	FORCEINLINE const FGameplayTag& GetLocomotionMode() const { return LocomotionMode; }
	FORCEINLINE const FGameplayTag& GetStance() const { return Stance; }
	FORCEINLINE const FGameplayTag& GetGait() const { return Gait; }

	void SetLocomotionAction(const FGameplayTag& NewLocomotionAction);
	void SetLocomotionMode(const FGameplayTag& NewLocomotionMode);
	void SetStance(const FGameplayTag& NewStance);
	void SetGait(const FGameplayTag& NewGait);

protected:

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	virtual void BeginPlay() override;

private:

	// -----------------------------
	// ------ TRAVERSALS ------
	// -----------------------------

	FTraversalResult CheckForTraversal();

	// ------ VAULT ------
	void HandleVault();

	// ------ MANTLE ------
	void HandleMantle();

	// ------ WALLRUN ------

	void HandleWallrun(float DeltaTime);

	bool HandleWallrunMovement(bool bIsLeftSide);
	void HandleWallrunCameraTilt(float DeltaTime);
	void HandleWallrunJump();

	void ResetWallrun();
	void EndWallrun();


	// ------ SLIDE ------

	void HandleSliding();
	bool ShouldCancelSliding();
	void CancelSliding();

public:

	// ------ LOCOMOTION ------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
		FGameplayTag LocomotionMode = OvrlLocomotionModeTags::Grounded;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
		FGameplayTag Stance = OvrlStanceTags::Standing;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
		FGameplayTag Gait = OvrlGaitTags::Walking;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
		FGameplayTag LocomotionAction = FGameplayTag::EmptyTag;

	// -----------------------------
	// ------ PARKOUR SECTION ------
	// -----------------------------

	// ------ TRAVERSAL VARS ------

	// The distance (forward and upward) to check if there are any traversals in front of us.
	// The forward check starts from the center of the player, whereas the upward check starts from the top of the player capsule
	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Traversal")
		FVector2D TraversalCheckDistance;

	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Traversal|Vault")
		float MaxVaultHeight;

	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Traversal|Vault")
		TObjectPtr<UAnimMontage> VaultMontage;

	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Traversal|Mantle")
		float MaxMantleHeight;

	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Traversal|Mantle")
		TObjectPtr<UAnimMontage> MantleMontage;

	// ------ WALLRUN VARS ------

	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Wallrun")
		float WallrunCheckDistance;

	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Wallrun")
		float WallrunCheckAngle;

	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Wallrun")
		float WallrunCameraTiltAngle;

	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Wallrun")
		float WallrunResetTime;

	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Wallrun")
		FVector WallrunJumpVelocity;


	// ------ SLIDING VARS ------

	// Vector that will be added to the player position, used to get the slope of the floor.
	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Sliding")
		float SlideDistanceCheck;

	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Sliding")
		float SlideForce;

	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Sliding")
		float SlideGroundFriction;

	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Sliding")
		float SlideBraking;

	UPROPERTY(EditAnywhere, Category = "Ovrl Character Movement Component|Sliding")
		float SlideMaxWalkSpeedCrouched;

private:

	// ------ DEFAULT VALUES ------

	UPROPERTY()
		TObjectPtr<AOvrlPlayerCharacter> Character;

	float DefaultGravity;
	float DefaultMaxWalkSpeed;
	float DefaultMaxWalkSpeedCrouched;
	float DefaultGroundFriction;
	float DefaultBrakingDecelerationWalking;

	// ------ WALLRUN VARS ------

	bool bCanCheckWallrun;
	FVector WallrunNormal;

	// ------ SLIDING VARS ------

	bool bShouldSlideOnLanded;
};

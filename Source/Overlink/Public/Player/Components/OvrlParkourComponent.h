// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OBM_ParkourComponent.generated.h"

enum EParkourMovementType : uint8
{
	None,
	WallrunLeft,
	WallrunRight,
	Sliding
};

class ACharacter;
class UCharacterMovementComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class OVERLINK_API UOBM_ParkourComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UOBM_ParkourComponent();

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	UFUNCTION(BlueprintCallable, Category = "ParkourComponent")
		void Initialize(ACharacter* InCharacter);

	UFUNCTION(BlueprintCallable, Category = "ParkourComponent")
		void OnPlayerJumped();

	UFUNCTION(BlueprintCallable, Category = "ParkourComponent")
		void OnPlayerLanded();

	UFUNCTION(BlueprintCallable, Category = "ParkourComponent")
		void HandleCrouching(bool bWantsToCrouch);

	FORCEINLINE bool IsWallrunning() const { return MovementType == EParkourMovementType::WallrunLeft || MovementType == EParkourMovementType::WallrunRight; };

	FORCEINLINE bool IsSliding() const { return MovementType == EParkourMovementType::Sliding; };

	FORCEINLINE EParkourMovementType GetMovementType() const { return MovementType; };

protected:

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

	// ------ WALLRUN VARS ------

	UPROPERTY(EditAnywhere, Category = "ParkourComponent|Wallrun")
		float WallrunCheckDistance;

	UPROPERTY(EditAnywhere, Category = "ParkourComponent|Wallrun")
		float WallrunCheckAngle;

	UPROPERTY(EditAnywhere, Category = "ParkourComponent|Wallrun")
		float WallrunCameraTiltAngle;

	UPROPERTY(EditAnywhere, Category = "ParkourComponent|Wallrun")
		float WallrunResetTime;

	UPROPERTY(EditAnywhere, Category = "ParkourComponent|Wallrun")
		FVector WallrunJumpVelocity;


	// ------ SLIDING VARS ------

	// Vector that will be added to the player position, used to get the slope of the floor.
	UPROPERTY(EditAnywhere, Category = "ParkourComponent|Sliding")
		float SlideDistanceCheck;

	UPROPERTY(EditAnywhere, Category = "ParkourComponent|Sliding")
		float SlideForce;

	UPROPERTY(EditAnywhere, Category = "ParkourComponent|Sliding")
		float SlideGroundFriction;

	UPROPERTY(EditAnywhere, Category = "ParkourComponent|Sliding")
		float SlideBraking;

	UPROPERTY(EditAnywhere, Category = "ParkourComponent|Sliding")
		float SlideMaxWalkSpeedCrouched;

private:

	// ------ DEFAULT VALUES ------

	UPROPERTY()
		TObjectPtr<ACharacter> Character;

	UPROPERTY()
		TObjectPtr<UCharacterMovementComponent> CharacterMovement;

	float DefaultGravity;
	float DefaultMaxWalkSpeed;
	float DefaultMaxWalkSpeedCrouched;
	float DefaultGroundFriction;
	float DefaultBrakingDecelerationWalking;


	// ------ WALLRUN VARS ------

	TEnumAsByte<EParkourMovementType> MovementType;
	bool bCanCheckWallrun;
	FVector WallrunNormal;

	// ------ SLIDING VARS ------

	bool bShouldSlideOnLanded;
};

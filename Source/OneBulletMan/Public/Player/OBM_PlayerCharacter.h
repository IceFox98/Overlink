// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "OBM_CharacterBase.h"
#include "AbilitySystemInterface.h"
#include "OBM_GameplayTags.h"

#include "OBM_PlayerCharacter.generated.h"

class UOBM_CameraComponent;
class UOBM_CharacterMovementComponent;
class UOBM_InteractionComponent;
class USceneComponent;
class UOBM_InventoryComponent;
class UOBM_ParkourComponent;
class UOBM_InputConfig;
class USpringArmComponent;
class UInputMappingContext;
struct FInputActionValue;

/**
 *
 */
UCLASS(Blueprintable)
class ONEBULLETMAN_API AOBM_PlayerCharacter : public AOBM_CharacterBase
{
	GENERATED_BODY()

public:

	// Sets default values for this character's properties
	AOBM_PlayerCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

public:

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

	// ------ MOVEMENT ------

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);

	virtual void Crouch(bool bClientSimulation) override;
	virtual void UnCrouch(bool bClientSimulation) override;

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode = 0) override;

	// ------ LOCOMOTION ------

	FORCEINLINE const FGameplayTag& GetLocomotionMode() const { return LocomotionMode; }
	FORCEINLINE const FGameplayTag& GetStance() const { return Stance; }
	FORCEINLINE const FGameplayTag& GetGait() const { return Gait; }
	FORCEINLINE const FGameplayTag& GetOverlayMode() const { return OverlayMode; }
	FORCEINLINE bool IsAiming() const { return bIsAiming; }

	void SetLocomotionMode(const FGameplayTag& NewLocomotionMode);
	void SetStance(const FGameplayTag& NewStance);
	void SetGait(const FGameplayTag& NewGait);
	void SetOverlayMode(const FGameplayTag& NewOverlayMode);

	UFUNCTION(BlueprintCallable, Category = "OBM|Player")
		void SetIsAiming(bool bNewIsAiming) { bIsAiming = bNewIsAiming; };

	// ------ INTERACTION ------

	void Interact();

	UFUNCTION(BlueprintCallable)
		void ThrowEquippedObject();

	UOBM_ParkourComponent* GetParkourComponent() const { return ParkourComponent; }

private:

	void OnAbilityInputPressed(FGameplayTag InputTag);

	void OnAbilityInputReleased(FGameplayTag InputTag);

protected:

	// ------ COMPONENTS ------

	/** FPS camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		TObjectPtr<UOBM_CameraComponent> CameraComp;

	/** Components that manages the interaction with usable actors */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		TObjectPtr<UOBM_InteractionComponent> InteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		TObjectPtr<UOBM_InventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		TObjectPtr<UOBM_ParkourComponent> ParkourComponent;

	// ------ LOCOMOTION ------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
		FGameplayTag LocomotionMode = OBM_LocomotionModeTags::Grounded;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
		FGameplayTag Stance = OBM_StanceTags::Standing;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
		FGameplayTag Gait = OBM_GaitTags::Walking;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
		FGameplayTag OverlayMode = OBM_OverlayModeTags::Default;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
		FGameplayTag LocomotionAction = FGameplayTag::EmptyTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
		bool bIsAiming;
public:

	// ------ INPUT ------

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OBM|Input")
		TObjectPtr<UInputMappingContext> DefaultMappingContext;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OBM|Input")
		TObjectPtr<UOBM_InputConfig> InputConfig;
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Throwing")
		float ThrowForce;
};

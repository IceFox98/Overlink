// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "OBM_CharacterBase.h"
#include "AbilitySystemInterface.h"
#include "Player/Components/OBM_CharacterMovementComponent.h"

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
struct FGameplayTag;

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

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	//	TObjectPtr<USpringArmComponent> CameraBoom;

	/** Components that manages the interaction with usable actors */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		TObjectPtr<UOBM_InteractionComponent> InteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		TObjectPtr<UOBM_InventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		TObjectPtr<UOBM_ParkourComponent> ParkourComponent;

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

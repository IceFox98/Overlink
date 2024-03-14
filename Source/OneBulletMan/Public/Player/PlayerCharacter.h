// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CharacterBase.h"
#include "AbilitySystemInterface.h"

#include "PlayerCharacter.generated.h"

class UCameraComponent;
class UInteractionComponent;
class UInventoryComponent;
class UOBM_InputConfig;

struct FInputActionValue;
struct FGameplayTag;

/**
 *
 */
UCLASS(Blueprintable)
class ONEBULLETMAN_API APlayerCharacter : public ACharacterBase
{
	GENERATED_BODY()

public:

	APlayerCharacter();

protected:
	virtual void BeginPlay() override;

public:

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

	// ------ MOVEMENT ------

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);

	// ------ MOVEMENT ------

	void Interact();

	UFUNCTION(BlueprintCallable)
		void ThrowEquippedObject();

	FORCEINLINE USceneComponent* GetItemHoldingPoint() const { return ItemHoldingPoint; }
	FORCEINLINE USceneComponent* GetCameraComp() const { return (USceneComponent*)CameraComp; }

private:

	void OnAbilityInputPressed(FGameplayTag InputTag);

	void OnAbilityInputReleased(FGameplayTag InputTag);

protected:

	// ------ COMPONENTS ------

	/** FPS camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UCameraComponent* CameraComp;

	/** Components that manages the interaction with usable actors */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UInteractionComponent* InteractionComponent;

	/** The location where the current selected object is hold */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
		class USceneComponent* ItemHoldingPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UInventoryComponent* InventoryComponent;

public:

	// ------ INPUT ------

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OBM|Input")
		class UInputMappingContext* DefaultMappingContext;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OBM|Input")
		TObjectPtr<UOBM_InputConfig> InputConfig;
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Throwing")
		float ThrowForce;
};

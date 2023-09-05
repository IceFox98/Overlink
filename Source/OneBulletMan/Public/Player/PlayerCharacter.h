// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "Misc/Guid.h"
#include "PlayerCharacter.generated.h"

class UCameraComponent;
class UInteractionComponent;
class UOBM_AbilitySystemComponent;
class UOBM_InputConfig;
class UOBM_AbilitySet;

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

	// Called when health is changed, either from healing or from being damaged
	UFUNCTION(BlueprintImplementableEvent)
		void OnHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	virtual void HandleHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	// ------ MOVEMENT ------

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);

	// ------ MOVEMENT ------

	void Interact();

	UFUNCTION(BlueprintCallable)
		void ThrowEquippedObject();

	FORCEINLINE USceneComponent* GetItemHoldingPoint() const { return ItemHoldingPoint; }
	FORCEINLINE USceneComponent* GetCameraComp() const { return (USceneComponent*)CameraComp; }

	UFUNCTION(BlueprintImplementableEvent)
		bool HasKey(FGuid Key);

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

	/** Components that manages the player abilities */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		TObjectPtr<UOBM_AbilitySystemComponent> AbilitySystemComponent;

	/** The location where the current selected object is hold */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
		class USceneComponent* ItemHoldingPoint;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	//	class UInventoryComponent* InventoryComponent;

public:

	// ------ COMPONENTS ------

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OBM|Input")
		class UInputMappingContext* DefaultMappingContext;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OBM|Input")
		TObjectPtr<UOBM_InputConfig> InputConfig;

	// Ability sets to grant to this pawn's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OBM|Abilities")
		TArray<TObjectPtr<UOBM_AbilitySet>> AbilitySets;

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Throwing")
		float ThrowForce;
};

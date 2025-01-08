// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CharacterBase.h"
#include "AbilitySystemInterface.h"
#include "Player/Components/OBM_CharacterMovementComponent.h"

#include "PlayerCharacter.generated.h"

class UOBM_CameraComponent;
class UOBM_CharacterMovementComponent;
class UInteractionComponent;
class USceneComponent;
class UInventoryComponent;
class UParkourComponent;
class UOBM_InputConfig;
class UInputMappingContext;
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

	// Sets default values for this character's properties
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

//
//protected:
//	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character")
//		UOBM_CharacterMovementComponent* GetOBMMovementComponent();

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

	//FORCEINLINE USceneComponent* GetItemHoldingPoint() const { return ItemHoldingPoint; }
	//FORCEINLINE USceneComponent* GetCameraComp() const { return CameraComp; }

	virtual USceneComponent* GetItemHoldingComponent() const override { return Cast<USceneComponent>(FirstPersonMesh); }

private:

	void OnAbilityInputPressed(FGameplayTag InputTag);

	void OnAbilityInputReleased(FGameplayTag InputTag);

protected:

	// ------ COMPONENTS ------

	//	/** Camera boom positioning the camera behind the character */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//	TObjectPtr<USpringArmComponent>* CameraBoom;

	/** FPS camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		TObjectPtr<UOBM_CameraComponent> CameraComp;

	/** Components that manages the interaction with usable actors */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		TObjectPtr<UInteractionComponent> InteractionComponent;

	///** The location where the current selected object is hold */
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	//	TObjectPtr<USceneComponent> ItemHoldingPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
		TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		TObjectPtr<UInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		TObjectPtr<UParkourComponent> ParkourComponent;

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

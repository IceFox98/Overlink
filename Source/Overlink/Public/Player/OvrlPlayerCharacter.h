// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "OvrlCharacterBase.h"
#include "AbilitySystemInterface.h"
#include "OvrlGameplayTags.h"

#include "OvrlPlayerCharacter.generated.h"

class UOvrlCameraComponent;
class UOvrlCharacterMovementComponent;
class UOvrlInteractionComponent;
class USceneComponent;
class UOvrlInventoryComponent;
class UOvrlParkourComponent;
class UOvrlInputConfig;
class USpringArmComponent;
class UInputMappingContext;
struct FInputActionValue;

/**
 *
 */
UCLASS(Blueprintable)
class OVERLINK_API AOvrlPlayerCharacter : public AOvrlCharacterBase
{
	GENERATED_BODY()

public:

	// Sets default values for this character's properties
	AOvrlPlayerCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

public:

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

	USkeletalMeshComponent* GetFPMesh() const { return FPMesh; };

	// ------ MOVEMENT ------

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);

	virtual void Crouch(bool bClientSimulation = false) override;
	virtual void UnCrouch(bool bClientSimulation = false) override;

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode = 0) override;

	// ------ LOCOMOTION ------

	FORCEINLINE const FGameplayTag& GetLocomotionAction() const { return LocomotionAction; }
	FORCEINLINE const FGameplayTag& GetLocomotionMode() const { return LocomotionMode; }
	FORCEINLINE const FGameplayTag& GetStance() const { return Stance; }
	FORCEINLINE const FGameplayTag& GetGait() const { return Gait; }
	FORCEINLINE const FGameplayTag& GetOverlayMode() const { return OverlayMode; }

	void SetLocomotionAction(const FGameplayTag& NewLocomotionAction);
	void SetLocomotionMode(const FGameplayTag& NewLocomotionMode);
	void SetStance(const FGameplayTag& NewStance);
	void SetGait(const FGameplayTag& NewGait);
	void SetOverlayMode(const FGameplayTag& NewOverlayMode);

	// ------ INTERACTION ------

	void Interact();

	UFUNCTION(BlueprintCallable)
		void ThrowEquippedObject();

	UOvrlParkourComponent* GetParkourComponent() const { return ParkourComponent; }

private:

	void OnAbilityInputPressed(FGameplayTag InputTag);

	void OnAbilityInputReleased(FGameplayTag InputTag);

protected:

	// ------ COMPONENTS ------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		TObjectPtr<USkeletalMeshComponent> FPMesh;

	//	/** FPS camera */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	//	TObjectPtr<USpringArmComponent> SpringArm;

	/** FPS camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		TObjectPtr<UOvrlCameraComponent> CameraComp;

	/** Components that manages the interaction with usable actors */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		TObjectPtr<UOvrlInteractionComponent> InteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		TObjectPtr<UOvrlInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		TObjectPtr<UOvrlParkourComponent> ParkourComponent;

	// ------ LOCOMOTION ------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
		FGameplayTag LocomotionMode = OvrlLocomotionModeTags::Grounded;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
		FGameplayTag Stance = OvrlStanceTags::Standing;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
		FGameplayTag Gait = OvrlGaitTags::Walking;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
		FGameplayTag OverlayMode = OvrlOverlayModeTags::Default;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", Transient)
		FGameplayTag LocomotionAction = FGameplayTag::EmptyTag;

public:

	// ------ INPUT ------

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl|Input")
		TObjectPtr<UInputMappingContext> DefaultMappingContext;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ovrl|Input")
		TObjectPtr<UOvrlInputConfig> InputConfig;
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Throwing")
		float ThrowForce;
};

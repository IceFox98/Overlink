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
class UMotionWarpingComponent;
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

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ovrl Player Character")
		UOvrlCharacterMovementComponent* GetCharacterMovement() const;

	UMotionWarpingComponent* GetMotionWarpingComponent() const { return MotionWarping; };
	UOvrlInventoryComponent* GetInventoryComponent() const { return InventoryComponent; };
	UOvrlCameraComponent* GetCameraComponent() const { return CameraComp; };

public:

	virtual USceneComponent* GetEquipAttachmentComponent() const override { return Cast<USceneComponent>(GetMesh()); }

	virtual void ApplyAnimClassLayer(const TSubclassOf<UOvrlLinkedAnimInstance>& LayerClass) override;

	void PlayAnimMontage(UAnimMontage* MontageToPlay, float StartTime = 0.f);

	// ------ MOVEMENT ------

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);

	virtual void Crouch(bool bClientSimulation = false) override;
	virtual void UnCrouch(bool bClientSimulation = false) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	// ------ LOCOMOTION ------

	FORCEINLINE const FGameplayTag& GetOverlayMode() const { return OverlayMode; }

	void SetOverlayMode(const FGameplayTag& NewOverlayMode);

	// ------ INTERACTION ------

	void Interact();

	UFUNCTION(BlueprintCallable)
		void ThrowEquippedObject();

private:

	void OnAbilityInputPressed(FGameplayTag InputTag);

	void OnAbilityInputReleased(FGameplayTag InputTag);

protected:

	// ------ COMPONENTS ------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		TObjectPtr<USpringArmComponent> SpringArm;

	/** FPS camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		TObjectPtr<UOvrlCameraComponent> CameraComp;

	/** Components that manages the interaction with usable actors */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		TObjectPtr<UOvrlInteractionComponent> InteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		TObjectPtr<UOvrlInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		TObjectPtr<UMotionWarpingComponent> MotionWarping;

	// ------ LOCOMOTION ------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
		FGameplayTag OverlayMode = OvrlOverlayModeTags::Default;

public:

	// ------ INPUT ------

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl|Input")
		TObjectPtr<UInputMappingContext> DefaultMappingContext;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ovrl|Input")
		TObjectPtr<UOvrlInputConfig> InputConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Throwing")
		float ThrowForce;
};

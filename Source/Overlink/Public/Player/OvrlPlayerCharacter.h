// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "OvrlCharacterBase.h"
#include "AbilitySystemInterface.h"
#include "OvrlGameplayTags.h"
#include "Core/Interfaces/OvrlSaveableObject.h"

#include "OvrlPlayerCharacter.generated.h"

class UOvrlCameraComponent;
class UOvrlInteractionComponent;
class UOvrlInventoryComponent;
class UOvrlEquipmentManagerComponent;
class UMotionWarpingComponent;
class UOvrlInputConfig;
class UInputMappingContext;
class AStaticMeshActor;
class UOvrlFoleyAudioBank;
class UAudioComponent;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerJumped);

/**
 *
 */
UCLASS(Blueprintable)
class OVERLINK_API AOvrlPlayerCharacter : public AOvrlCharacterBase, public IOvrlSaveableObject
{

private:
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AOvrlPlayerCharacter(const FObjectInitializer& ObjectInitializer);

public:
	virtual void OnJumped_Implementation() override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual void PostInitializeComponents() override;
	
	// ---- IOvrlSaveableObject interface
	virtual bool SaveAsPlayer_Implementation() override { return true; };
	// ---- IOvrlSaveableObject interface
	
protected:
	virtual void BeginPlay() override;
	virtual bool CanJumpInternal_Implementation() const override;

public:
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UMotionWarpingComponent* GetMotionWarpingComponent() const { return MotionWarping; };
	UOvrlInventoryComponent* GetInventoryComponent() const { return InventoryComponent; };
	UOvrlEquipmentManagerComponent* GetEquipmentManagerComponent() const { return EquipmentManagerComponent; };
	UOvrlCameraComponent* GetCameraComponent() const { return CameraComp; };
	USkeletalMeshComponent* GetFullBodyMesh() const { return FullBodyMesh; };

public:
	virtual void ApplyAnimLayerClass(const TSubclassOf<UOvrlLinkedAnimInstance>& LayerClass) override;
	virtual void RestoreAnimLayerClass() override;
	virtual void EquipObject(AActor* ObjectToEquip, FName AttachSocketName, UStaticMesh* MeshToDisplay) override;
	virtual void UnequipObject() override;
	virtual void OvrlPlayAnimMontage(UAnimMontage* MontageToPlay, float StartTime = 0.f) override;
	virtual void OvrlStopAnimMontage(UAnimMontage* MontageToStop) override;

protected:
	void PlayLandSound() const;
	void PlayJumpSound() const;

	// ------ MOVEMENT ------

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);
	void Input_Crouch(const FInputActionValue& InputActionValue);

	// ------ INTERACTION ------
	void Input_StartInteract(const FInputActionValue& InputActionValue);
	void Input_EndInteract(const FInputActionValue& InputActionValue);

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

protected:
	bool CheckWallCollisions(const FVector& Direction);

	UFUNCTION()
	void OnLocomotionActionChanged(const FGameplayTag& OldLocomotionAction, const FGameplayTag& NewLocomotionAction);

private:
	void OnAbilityInputStarted(FGameplayTag InputTag);
	void OnAbilityInputTriggered(FGameplayTag InputTag);
	void OnAbilityInputReleased(FGameplayTag InputTag);

protected:
	// ------ COMPONENTS ------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> FullBodyMesh;

	/** FPS camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UOvrlCameraComponent> CameraComp;

	/** Components that manages the interaction with usable actors */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UOvrlInteractionComponent> InteractionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UOvrlInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UOvrlEquipmentManagerComponent> EquipmentManagerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UMotionWarpingComponent> MotionWarping;

public:
	UPROPERTY(BlueprintAssignable)
	FOnPlayerJumped OnPlayerJumped;

	// If true, the fist player camera will be stabilized, ignoring every player's body movements.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Player Character")
	bool bEnableCameraStabilization;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Player Character|Audio")
	TObjectPtr<UOvrlFoleyAudioBank> FoleyAudioBank;

	UPROPERTY(EditAnywhere, Category = "Ovrl Player Character|Audio")
	float LandSoundMultiplier;

	UPROPERTY(EditAnywhere, Category = "Ovrl Player Character|Audio")
	float JumpSoundMultiplier;

	UPROPERTY(EditAnywhere, Category = "Ovrl Player Character|Audio")
	float SlideSoundMultiplier;

	// ------ INPUT ------

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ovrl Player Character|Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ovrl Player Character|Input")
	TObjectPtr<UOvrlInputConfig> InputConfig;

private:
	UPROPERTY()
	TObjectPtr<AStaticMeshActor> EquippedObjectMesh;

	UPROPERTY()
	TObjectPtr<UAudioComponent> SlidingAudioComponent;
};

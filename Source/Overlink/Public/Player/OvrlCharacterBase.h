// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"

#include "OvrlCharacterBase.generated.h"

class UOvrlHealthComponent;
class UOvrlCharacterMovementComponent;
class UOvrlAbilitySystemComponent;
class UOvrlAbilitySet;
class UOvrlLinkedAnimInstance;

UCLASS()
class OVERLINK_API AOvrlCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AOvrlCharacterBase(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ovrl Character Base")
	UOvrlCharacterMovementComponent* GetCharacterMovement() const;
	virtual UOvrlAbilitySystemComponent* GetOvrlAbilitySystemComponent() const { return AbilitySystemComponent; };
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION()
	virtual void HandleDeath(AActor* InInstigator);
	virtual void Landed(const FHitResult& Hit) override;

	virtual void ApplyAnimLayerClass(const TSubclassOf<UOvrlLinkedAnimInstance>& LayerClass);
	virtual void RestoreAnimLayerClass();
	virtual void EquipObject(AActor* ObjectToEquip, FName AttachSocketName, UStaticMesh* MeshToDisplay);
	virtual void UnequipObject();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ovrl Character Base")
	FORCEINLINE bool HasDefaultAnimLayerClass() const { return bHasDefaultAnimLayerClass; };

	UFUNCTION()
	virtual void OvrlPlayAnimMontage(UAnimMontage* MontageToPlay, float StartTime = 0.f);
	virtual void OvrlStopAnimMontage(UAnimMontage* MontageToStop);

protected:
	// ------ COMPONENTS ------

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UOvrlAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UOvrlHealthComponent> HealthComponent;

public:
	// Ability sets to grant to this pawn's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ovrl Character Base")
	TArray<TObjectPtr<UOvrlAbilitySet>> AbilitySets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ovrl Character Base")
	TSubclassOf<UOvrlLinkedAnimInstance> DefaultAnimLayerClass;

	// Time (in seconds) before bJustLanded is reset.
	UPROPERTY(EditAnywhere, Category = "Ovrl Character Base")
	float LandedResetTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl Character Base", Transient)
	bool bJustLanded;

protected:
	bool bHasDefaultAnimLayerClass;

private:
	FTimerHandle TimerHandle_LandReset;

};

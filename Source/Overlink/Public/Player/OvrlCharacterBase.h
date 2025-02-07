// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"

#include "OvrlCharacterBase.generated.h"

class UOvrlHealthComponent;
class UOvrlAbilitySystemComponent;
class UOvrlAbilitySet;

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
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return (UAbilitySystemComponent*)AbilitySystemComponent; };

	virtual USceneComponent* GetItemHoldingComponent() const { return Cast<USceneComponent>(GetMesh()); }

	UFUNCTION()
		virtual void HandleDeath(AActor* InInstigator);

protected:

	/** Components that manages the player abilities */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl|Components")
		TObjectPtr<UOvrlAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl|Components")
		UOvrlHealthComponent* HealthComponent;

public:

	// Ability sets to grant to this pawn's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ovrl|Abilities")
		TArray<TObjectPtr<UOvrlAbilitySet>> AbilitySets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ovrl|Grip")
		FName GripPointName;
};

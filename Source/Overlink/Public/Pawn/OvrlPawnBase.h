// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "AbilitySystemInterface.h"

#include "OvrlPawnBase.generated.h"

class UOvrlHealthComponent;
class UOvrlAbilitySystemComponent;
class UOvrlAbilitySet;

UCLASS()
class OVERLINK_API AOvrlPawnBase : public APawn, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AOvrlPawnBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION()
	virtual void HandleDeath(AActor* InInstigator);

protected:

	/** Components that manages the player abilities */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl|Components")
	TObjectPtr<UOvrlAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ovrl|Components")
	TObjectPtr<UOvrlHealthComponent> HealthComponent;

public:

	// Ability sets to grant to this pawn's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ovrl|Abilities")
	TArray<TObjectPtr<UOvrlAbilitySet>> AbilitySets;
};

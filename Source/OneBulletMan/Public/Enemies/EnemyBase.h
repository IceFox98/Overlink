// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "AbilitySystemInterface.h"

#include "EnemyBase.generated.h"

class UHealthComponent;
class UOBM_AbilitySystemComponent;
class UOBM_AbilitySet;

UCLASS()
class ONEBULLETMAN_API AEnemyBase : public APawn, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemyBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { 
		return Cast<UAbilitySystemComponent>(AbilitySystemComponent); 
	};

protected:

	/** Components that manages the player abilities */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		TObjectPtr<UOBM_AbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UHealthComponent* HealthComp;

public:

	// Ability sets to grant to this pawn's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "OBM|Abilities")
		TArray<TObjectPtr<UOBM_AbilitySet>> AbilitySets;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"

#include "OvrlCharacterBase.generated.h"

class UOvrlHealthComponent;
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
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	virtual UOvrlAbilitySystemComponent* GetOvrlAbilitySystemComponent() const { return AbilitySystemComponent; };
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	//virtual USceneComponent* GetEquipAttachmentComponent() const { return Cast<USceneComponent>(GetMesh()); }

	UFUNCTION()
		virtual void HandleDeath(AActor* InInstigator);

	virtual void ApplyAnimClassLayer(const TSubclassOf<UOvrlLinkedAnimInstance>& LayerClass) { unimplemented(); };
	virtual void EquipObject(AActor* ObjectToEquip, UStaticMesh* MeshToDisplay);

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

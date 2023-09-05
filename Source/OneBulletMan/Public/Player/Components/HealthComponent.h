// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

// OnHealthChanged event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, UHealthComponent*, HealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS(ClassGroup = (Custom))
class ONEBULLETMAN_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UHealthComponent();

private:


protected:

	/** The current Health */
	UPROPERTY(VisibleAnywhere, Category = "Health Component")
		float Health;

	UPROPERTY(EditAnywhere, Category = "Health Component")
		float MaxHealth;

	UPROPERTY(VisibleAnywhere, Category = "Health Component")
		bool bIsDead;

public:

	UPROPERTY(BlueprintAssignable, Category = "Events")
		FOnHealthChangedSignature OnHealthChanged;

	/**
	 * FUNCTIONS
	 */
private:


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
		void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:

	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float IsDead() const { return bIsDead; }

	UFUNCTION(BlueprintCallable, Category = "Health Component")
		void Heal(float HealAmount);

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OBM_HealthComponent.generated.h"

class UOBM_HealthSet;
class UOBM_AbilitySystemComponent;
struct FOnAttributeChangeData;

// OnHealthChanged event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHealthChangedSignature, UOBM_HealthComponent*, HealthComp, float, OldValue, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOutOfHealthSignature, AActor*, Instigator);

UCLASS(ClassGroup = (Custom))
class OVERLINK_API UOBM_HealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UOBM_HealthComponent();

public:

	void InitializeWithASC(UOBM_AbilitySystemComponent* ASC);

protected:

	void HandleHealthChanged(const FOnAttributeChangeData& Data);

	void HandleMaxHealthChanged(const FOnAttributeChangeData& Data);

public:

	UPROPERTY(BlueprintAssignable, Category = "Health Component|Events")
		FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Health Component|Events")
		FOnHealthChangedSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Health Component|Events")
		FOnOutOfHealthSignature OnOutOfHealth;
public:

	// Ability system used by this component.
	UPROPERTY()
		TObjectPtr<UOBM_AbilitySystemComponent> AbilitySystemComponent;

	// Health set used by this component.
	UPROPERTY()
		TObjectPtr<const UOBM_HealthSet> HealthSet;

	UPROPERTY(VisibleAnywhere, Category = "Health Component")
		bool bIsDead;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OvrlHealthComponent.generated.h"

class UOvrlHealthSet;
class UOvrlAbilitySystemComponent;
struct FOnAttributeChangeData;

// OnHealthChanged event
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnHealthChangedSignature, UOvrlHealthComponent*, HealthComp, float, OldValue, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOutOfHealthSignature, AActor*, Instigator);

UCLASS(ClassGroup = (Custom))
class OVERLINK_API UOvrlHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UOvrlHealthComponent();

public:

	void InitializeWithASC(UOvrlAbilitySystemComponent* ASC);

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
	TObjectPtr<UOvrlAbilitySystemComponent> AbilitySystemComponent;

	// Health set used by this component.
	UPROPERTY()
	TObjectPtr<const UOvrlHealthSet> HealthSet;

	UPROPERTY(VisibleAnywhere, Category = "Health Component")
	bool bIsDead;
};

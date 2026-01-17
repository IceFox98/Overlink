// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/OvrlAttributeSet.h"

#include "OvrlHealthSet.generated.h"

/**
 *
 */
UCLASS()
class OVERLINK_API UOvrlHealthSet : public UOvrlAttributeSet
{
	GENERATED_BODY()

public:

	UOvrlHealthSet();

	ATTRIBUTE_ACCESSORS(UOvrlHealthSet, Health);
	ATTRIBUTE_ACCESSORS(UOvrlHealthSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UOvrlHealthSet, Damage);

protected:

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:

	// The current health attribute.  The health will be capped by the max health attribute.  Health is hidden from modifiers so only executions can modify it.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Ovrl|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	// The current max health attribute.  Max health is an attribute since gameplay effects can modify it.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Ovrl|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	// -------------------
	//	Meta Attributes 
	// -------------------

private:

	// NOT USED, JUST FOR LEARNING PURPOSE
	// 
	// 
	// Incoming damage. This is mapped directly to -Health
	// Damage is a meta attribute used by the DamageExecution to calculate final damage.
	// Temporary value that only exists on the Server. Not replicated.
	// The reason why we want a temporary value rather than directly modify the damage is because we might have other
	// abilities that modify damage, E.g.: Armor
	UPROPERTY(BlueprintReadOnly, Category = "Ovrl|Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Damage;

};

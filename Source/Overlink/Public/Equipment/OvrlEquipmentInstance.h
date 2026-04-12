// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystem/OvrlAbilitySet.h"
#include "OvrlEquipmentInstance.generated.h"

class UOvrlEquipmentDefinition;
class UOvrlInventoryComponent;
class UOvrlItemInstance;
class UCurveVector;

UCLASS()
class OVERLINK_API AOvrlEquipmentInstance : public AActor
{
	GENERATED_BODY()

public:
	AOvrlEquipmentInstance();

	virtual void Destroyed() override;

protected:
	virtual void BeginPlay() override;

public:
	void Initialize(const TSubclassOf<UOvrlEquipmentDefinition>& InEquipmentDefinitionClass, UOvrlItemInstance* InAssociatedItem);

	UFUNCTION(BlueprintNativeEvent, Category = "Ovrl Equipment Instance")
	void OnEquipped();
	virtual void OnEquipped_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Ovrl Equipment Instance")
	void OnBeforeUnequip();
	virtual void OnBeforeUnequip_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Ovrl Equipment Instance")
	void OnUnequipped();
	virtual void OnUnequipped_Implementation();

	UFUNCTION(BlueprintCallable, Category = "Ovrl Equipment Instance")
	FORCEINLINE UOvrlItemInstance* GetAssociatedItem() const { return AssociatedItem; };

	FORCEINLINE bool IsEquipped() const { return bIsEquipped; };

	UFUNCTION(BlueprintCallable, Category = "Ovrl Equipment Instance")
	virtual FTransform GetLeftHandIKTransform() const { return FTransform(); };

	float GetEquipNotifyTime() const;
	void PlayEquipMontage() const;

protected:
	void ApplyOverlayAnimInstance() const;

protected:
	// The equipment class that got equipped
	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Instance")
	TSubclassOf<UOvrlEquipmentDefinition> EquipmentDefinitionClass;

	UPROPERTY(VisibleInstanceOnly, Category = "Ovrl Equipment Instance")
	TObjectPtr<UOvrlItemInstance> AssociatedItem;

private:
	friend class UOvrlInventoryComponent;

	// List of granted handles
	FOvrlAbilitySet_GrantedHandles GrantedHandles;

	bool bIsEquipped = false;

	UPROPERTY()
	TObjectPtr<USceneComponent> TargetToFollow;

	UPROPERTY()
	TWeakObjectPtr<UStaticMesh> DisplayMesh;

	FVector RelativeLocation;
};

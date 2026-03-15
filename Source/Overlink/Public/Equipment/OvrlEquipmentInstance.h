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

UCLASS(meta = (Category = "Ovrl Equipment Instance"))
class OVERLINK_API AOvrlEquipmentInstance : public AActor
{
	GENERATED_BODY()

public:
	virtual void Destroyed() override;

public:
	virtual void OnEquipped();
	virtual void OnUnequipped();

	UFUNCTION(BlueprintImplementableEvent, Category = "Ovrl Equipment Instance", meta = (DisplayName = "On Equipped"))
	void K2_OnEquipped();

	UFUNCTION(BlueprintImplementableEvent, Category = "Ovrl Equipment Instance", meta = (DisplayName = "On Unequipped"))
	void K2_OnUnequipped();

	UFUNCTION(BlueprintCallable, Category = "Ovrl Equipment Instance")
	FORCEINLINE UOvrlItemInstance* GetAssociatedItem() const { return AssociatedItem; };
	FORCEINLINE void SetDisplayMesh(UStaticMesh* InDisplayMesh) { DisplayMesh = InDisplayMesh; };

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

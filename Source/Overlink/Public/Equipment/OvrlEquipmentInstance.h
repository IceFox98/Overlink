// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystem/OvrlAbilitySet.h"
#include "OvrlEquipmentInstance.generated.h"

class UOvrlEquipmentDefinition;
class UOvrlItemInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDropReady, const AOvrlEquipmentInstance*, ItemToDrop);

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
	void Initialize(UOvrlEquipmentDefinition* InEquipmentDefinition, UOvrlItemInstance* InAssociatedItem);

	// Called after the equipment animations.
	UFUNCTION(BlueprintNativeEvent, Category = "Ovrl Equipment Instance")
	void OnEquipped();
	virtual void OnEquipped_Implementation();

	// Called just before equip animations, useful to end logic of this item during the equipping process.
	UFUNCTION(BlueprintNativeEvent, Category = "Ovrl Equipment Instance")
	void OnBeforeUnequip();
	virtual void OnBeforeUnequip_Implementation();

	// Called after the equip animations, before a new item will be equipped.
	UFUNCTION(BlueprintNativeEvent, Category = "Ovrl Equipment Instance")
	void OnUnequipped();
	virtual void OnUnequipped_Implementation();

	// Called before the actual drop of the item (that consists in a Pickup Actor spawned in the scene).
	// Use this function to execute any logic before this item will be destroyed and dropped.
	UFUNCTION(BlueprintNativeEvent, Category = "Ovrl Equipment Instance")
	void BeginDrop();
	virtual void BeginDrop_Implementation();

	UFUNCTION(BlueprintCallable, Category = "Ovrl Equipment Instance")
	FORCEINLINE UOvrlItemInstance* GetAssociatedItem() const { return AssociatedItem; };

	FORCEINLINE bool IsEquipped() const { return bIsEquipped; };

	UFUNCTION(BlueprintCallable, Category = "Ovrl Equipment Instance")
	virtual FTransform GetLeftHandIKTransform() const { return FTransform(); };

	UFUNCTION(BlueprintCallable, Category = "Ovrl Equipment Instance")
	const UOvrlEquipmentDefinition* GetEquipmentDefinition() const { return EquipmentDefinition; };

	float GetEquipNotifyTime() const;
	void PlayEquipMontage() const;

protected:
	void ApplyOverlayAnimInstance() const;

	UOvrlAbilitySystemComponent* GetOwnerAbilitySystemComponent() const;

public:
	FOnDropReady OnDropReady;

protected:
	// The equipment class that got equipped
	UPROPERTY(BlueprintReadOnly, Category = "Ovrl Equipment Instance")
	TObjectPtr<UOvrlEquipmentDefinition> EquipmentDefinition;

	UPROPERTY(VisibleInstanceOnly, Category = "Ovrl Equipment Instance")
	TObjectPtr<UOvrlItemInstance> AssociatedItem;

	// Should be the skeletal mesh of the character holding the equipment.
	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> OwnerSkeletalMesh;

private:
	// List of granted handles
	FOvrlAbilitySet_GrantedHandles GrantedHandles;

	bool bIsEquipped = false;

	UPROPERTY()
	TObjectPtr<USceneComponent> TargetToFollow;

	UPROPERTY()
	TWeakObjectPtr<UStaticMesh> DisplayMesh;

	FVector RelativeLocation;
};

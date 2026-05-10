// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "OvrlEquipmentInstance.generated.h"

class UOvrlEquipmentDefinition;
class UOvrlItemInstance;

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

	UFUNCTION(BlueprintCallable, Category = "Ovrl Equipment Instance")
	FORCEINLINE UOvrlItemInstance* GetAssociatedItem() const { return AssociatedItem; };

	UFUNCTION(BlueprintCallable, Category = "Ovrl Equipment Instance")
	FORCEINLINE bool IsEquipped() const { return bIsEquipped; };

	UFUNCTION(BlueprintCallable, Category = "Ovrl Equipment Instance")
	virtual FTransform GetLeftHandIKTransform() const { return FTransform(); };

	UFUNCTION(BlueprintCallable, Category = "Ovrl Equipment Instance")
	const UOvrlEquipmentDefinition* GetEquipmentDefinition() const { return EquipmentDefinition; };

	float GetEquipNotifyTime() const;
	void PlayEquipMontage() const;

protected:
	void ApplyOverlayAnimInstance() const;

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
	UPROPERTY()
	TWeakObjectPtr<UStaticMesh> DisplayMesh;

	bool bIsEquipped = false;
	FVector RelativeLocation;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OvrlItemUtils.generated.h"

class UOvrlItemInstance;
class UOvrlPickupDefinition;
struct FOvrlItemEntry;

/**
 *
 */
UCLASS()
class OVERLINK_API UOvrlItemUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Looks for an Inventory Component and retrieve the first item matching the passed definition.
	UFUNCTION(BlueprintPure, Category = "Ovrl Item Utils")
	static UPARAM(DisplayName="Item") FOvrlItemEntry GetFirstItemEntry(AActor* InventoryOwner, TSubclassOf<UOvrlItemDefinition> ItemDefinition);

	// Looks for an Equipment Manager Component and retrieve the first equipment instance matching the passed definition.
	UFUNCTION(BlueprintPure, Category = "Ovrl Item Utils")
	static UPARAM(DisplayName="Equipment Instance") AOvrlEquipmentInstance* GetFirstEquipmentInstance(AActor* ManagerOwner, TSubclassOf<UOvrlItemDefinition> ItemDefinition);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ovrl Equipment Manager Component")
	static const UOvrlEquipmentDefinition* GetItemEquipmentDefinition(const UOvrlItemInstance* Item);
	
	// Searches an item definition type for a matching stat and returns the value, or 0 if not found
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ovrl Inventory Component")
	static int32 GetDefaultStatFromItemDef(const TSubclassOf<UOvrlItemDefinition> WeaponItemClass, FGameplayTag StatTag);
	
	UFUNCTION(BlueprintPure, Category = "Ovrl Item Utils")
	static UOvrlPickupDefinition* GetPickupDefinitionFromItemDefinition(TSubclassOf<UOvrlItemDefinition> ItemDefinitionClass);	
	
	UFUNCTION(BlueprintPure, Category = "Ovrl Item Utils")
	static UOvrlPickupDefinition* GetPickupDefinitionFromItemInstance(const UOvrlItemInstance* ItemInstance);
};

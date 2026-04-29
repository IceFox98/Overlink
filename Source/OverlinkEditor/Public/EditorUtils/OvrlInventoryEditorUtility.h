// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OvrlInventoryEditorUtility.generated.h"

class UOvrlItemDefinition;
class UOvrlEquipmentDefinition;
class AOvrlEquipmentInstance;
class UStaticMesh;

USTRUCT(BlueprintType)
struct FInventoryItemData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString FolderPath = "/Game";

	UPROPERTY(BlueprintReadWrite)
	FString AssetName;

	UPROPERTY(BlueprintReadWrite)
	FText DisplayName;

	UPROPERTY(BlueprintReadWrite)
	UStaticMesh* DisplayMesh;

	UPROPERTY(BlueprintReadWrite)
	UTexture2D* DisplayTexture;

	UPROPERTY(BlueprintReadWrite)
	bool bIsEquippable;

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<AOvrlEquipmentInstance> EquipmentClass;

	UPROPERTY(BlueprintReadWrite)
	USkeletalMesh* WeaponMesh;

	UPROPERTY(BlueprintReadWrite)
	FName AttachSocketName;

	UPROPERTY(BlueprintReadWrite)
	float BaseDamage;
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class OVERLINKEDITOR_API UOvrlInventoryUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf="WorldContextObject", HideSelfPin))
	static void CreateItem(UObject* WorldContextObject, const FInventoryItemData& ItemData);

protected:
	static void SaveObject(UObject* Object, const FString& PackagePath, UPackage* Package);

	static UPackage* CreateItemPackage(const FString& FolderPath, const FString& PackageName);

	static UBlueprint* CreateEquipmentInstance(const FInventoryItemData& ItemData);
	static UBlueprint* CreateEquipmentDefinition(const FInventoryItemData& ItemData, const UBlueprint* EquipmentInstanceBP);
	static UBlueprint* CreateItemDefinition(const FInventoryItemData& ItemData, const UBlueprint* EquipmentDefinitionBP);

private:
	static void ReopenObject(UObject* Object);

};

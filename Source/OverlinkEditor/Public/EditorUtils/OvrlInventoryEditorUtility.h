// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OvrlInventoryEditorUtility.generated.h"

class SMessageDialog;
class AOvrlItemPickupActor;
class UGameplayEffect;
class AOvrlProjectile;
class UOvrlItemAmmoBase;
class UOvrlAbilitySet;
class UOvrlItemDefinition;
class UOvrlEquipmentDefinition;
class AOvrlEquipmentInstance;
class UStaticMesh;

USTRUCT(BlueprintType)
struct FInventoryItemData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	bool bAlwaysOverwrite;

	UPROPERTY(BlueprintReadWrite)
	FString FolderPath = "/Game";

	UPROPERTY(BlueprintReadWrite)
	FString AssetName;

	UPROPERTY(BlueprintReadWrite)
	FText DisplayName;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UStaticMesh> DisplayMesh;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UTexture2D> DisplayTexture;

	UPROPERTY(BlueprintReadWrite)
	bool bIsEquippable;
	
	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<AOvrlItemPickupActor> PickupActorClass;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UOvrlAbilitySet> AbilitySet;

	UPROPERTY(BlueprintReadWrite)
	bool bAllowQuickSlot;

	UPROPERTY(BlueprintReadWrite)
	bool bShouldSpawnInstance;

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<AOvrlEquipmentInstance> EquipmentInstanceClass;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<USkeletalMesh> WeaponMesh;

	UPROPERTY(BlueprintReadWrite)
	FName AttachSocketName;

	UPROPERTY(BlueprintReadWrite)
	float BaseDamage = 10.f;

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> GE_DamageClass;

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UOvrlItemAmmoBase> AmmoType;

	UPROPERTY(BlueprintReadWrite)
	int32 BulletsPerCartridge = 1;

	UPROPERTY(BlueprintReadWrite)
	int32 MagazineSize = 30;
	
	UPROPERTY(BlueprintReadWrite)
	float FireRate = 400.f;

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<AOvrlProjectile> ProjectileClass;
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

private:
	static void CreateItemInternal(UObject* WorldContextObject, const FInventoryItemData& ItemData);
	
	static UBlueprint* CreateEquipmentInstance(const FInventoryItemData& ItemData);
	static UBlueprint* CreateEquipmentDefinition(const FInventoryItemData& ItemData, const UBlueprint* EquipmentInstanceBP);
	static UBlueprint* CreateItemDefinition(const FInventoryItemData& ItemData, const UBlueprint* EquipmentDefinitionBP);
	static UDataAsset* CreatePickupDefinition(const FInventoryItemData& ItemData, const UBlueprint* ItemDefinitionBP);
	static UBlueprint* CreatePickupActor(const FInventoryItemData& ItemData, UDataAsset* PickupDefinitionDA);

	static UBlueprint* FindOrCreateBlueprint(const FInventoryItemData& ItemData, const FString& Prefix, TSubclassOf<UObject> ParentClass);
	static UDataAsset* FindOrCreateDataAsset(const FInventoryItemData& ItemData, const FString& Prefix, TSubclassOf<UObject> ParentClass);

	static void ReopenObject(UObject* Object);
	
	static TSharedRef<SMessageDialog> ShowDuplicatedAssetDialog(const FString& AssetName, const FString& PackageName, const UObject* Asset);
	static TSharedRef<SMessageDialog> ShowMessageDialog(const FString& Title, const FText& Message, const FString& SubmitButtonText, bool bShowCancelButton = false);
private:
	
	static FInventoryItemData ItemDataRef;

};

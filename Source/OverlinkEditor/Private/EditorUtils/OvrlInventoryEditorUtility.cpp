// Fill out your copyright notice in the Description page of Project Settings.

#include "EditorUtils/OvrlInventoryEditorUtility.h"

#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Equipment/OvrlEquipmentDefinition.h"
#include "Factories/BlueprintFactory.h"
#include "Inventory/OvrlItemDefinition.h"
#include "Inventory/OvrlItemFragment_EquippableItem.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Weapons/OvrlWeaponInstance.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "UObject/SavePackage.h"

void UOvrlInventoryUtils::CreateItem(UObject* WorldContextObject, const FInventoryItemData& ItemData)
{
	UBlueprint* EquipmentInstanceBP = CreateEquipmentInstance(ItemData);
	UBlueprint* EquipmentDefinitionBP = CreateEquipmentDefinition(ItemData, EquipmentInstanceBP);
	UBlueprint* ItemDef = CreateItemDefinition(ItemData, EquipmentDefinitionBP);
}

UBlueprint* UOvrlInventoryUtils::CreateEquipmentInstance(const FInventoryItemData& ItemData)
{
	const FString AssetName = "BP_" + ItemData.AssetName;
	// UPackage* Package = CreateItemPackage(ItemData.FolderPath, PackageName);
	//
	// if (!Package)
	// {
	// 	return nullptr;
	// }
	//
	// const FString PackagePath = Package->GetName();
	//
	// if (!ItemData.EquipmentClass)
	// {
	// 	return nullptr;
	// }
	//
	// UBlueprint* EquipmentBlueprint = FindObject<UBlueprint>(Package, *PackageName);
	// if (!EquipmentBlueprint)
	// {
	// 	// Create equippable
	// 	EquipmentBlueprint = FKismetEditorUtilities::CreateBlueprint(
	// 		ItemData.EquipmentClass,
	// 		Package,
	// 		FName(*PackageName),
	// 		BPTYPE_Normal,
	// 		UBlueprint::StaticClass(),
	// 		UBlueprintGeneratedClass::StaticClass()
	// 	);
	// }
	//
	// if (!EquipmentBlueprint)
	// {
	// 	return nullptr;
	// }
	//
	// AOvrlWeaponInstance* DefaultActor = Cast<AOvrlWeaponInstance>(EquipmentBlueprint->GeneratedClass->GetDefaultObject());
	//
	// if (DefaultActor)
	// {
	// 	if (DefaultActor->WeaponMesh)
	// 	{
	// 		DefaultActor->WeaponMesh->SetSkeletalMesh(ItemData.WeaponMesh);
	// 	}
	//
	// 	DefaultActor->SetBaseDamage(ItemData.BaseDamage);
	// }
	//
	// FKismetEditorUtilities::CompileBlueprint(EquipmentBlueprint);
	//
	// SaveObject(EquipmentBlueprint, PackagePath, Package);

	// Load necessary modules
	// FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	// FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	// IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	// Generate a unique asset name
	// FString Name;
	// FString PackageName;
	// AssetToolsModule.Get().CreateUniqueAssetName(ItemData.FolderPath + "/" + AssetName, TEXT(""), PackageName, Name);
	// Name = AssetName;
	const FString PackageName = ItemData.FolderPath + "/" + AssetName;

	UPackage* Package = CreatePackage(*PackageName);
	UBlueprint* EquipmentBlueprint = FindObject<UBlueprint>(Package, *AssetName);

	if (!EquipmentBlueprint)
	{
		FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");

		const FString PackagePath = FPackageName::GetLongPackagePath(PackageName);
		// Create object and package
		UBlueprintFactory* MyFactory = NewObject<UBlueprintFactory>(UBlueprintFactory::StaticClass());
		MyFactory->bEditAfterNew = true;
		MyFactory->SupportedClass = UBlueprint::StaticClass();
		MyFactory->ParentClass = ItemData.EquipmentClass;
		EquipmentBlueprint = Cast<UBlueprint>(AssetToolsModule.Get().CreateAsset(AssetName, PackagePath, UBlueprint::StaticClass(), MyFactory));

		// FSavePackageArgs SavePackageArgs;
		// SavePackageArgs.TopLevelFlags = EObjectFlags::RF_Standalone | EObjectFlags::RF_Public;
		// UPackage::Save(Package, EquipmentBlueprint, *FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension()), SavePackageArgs);
		//
		// // Inform asset registry
		// AssetRegistry.AssetCreated(EquipmentBlueprint);

		FAssetRegistryModule::AssetCreated(EquipmentBlueprint);
		//ReopenObject(EquipmentBlueprint);
		SaveObject(EquipmentBlueprint, PackageName, Package);
	}

	if (!EquipmentBlueprint)
	{
		return nullptr;
	}

	AOvrlWeaponInstance* WeaponInstance = Cast<AOvrlWeaponInstance>(EquipmentBlueprint->GeneratedClass->GetDefaultObject());

	if (WeaponInstance)
	{
		if (WeaponInstance->WeaponMesh)
		{
			WeaponInstance->WeaponMesh->SetSkeletalMesh(ItemData.WeaponMesh);
		}

		WeaponInstance->SetBaseDamage(ItemData.BaseDamage);
	}

	FBlueprintEditorUtils::MarkBlueprintAsModified(EquipmentBlueprint);
	FKismetEditorUtilities::CompileBlueprint(EquipmentBlueprint);

	ReopenObject(EquipmentBlueprint);

	return EquipmentBlueprint;
}

UBlueprint* UOvrlInventoryUtils::CreateEquipmentDefinition(const FInventoryItemData& ItemData, const UBlueprint* EquipmentInstanceBP)
{
	const FString AssetName = "ED_" + ItemData.AssetName;
	const FString PackageName = ItemData.FolderPath + "/" + AssetName;

	UPackage* Package = CreatePackage(*PackageName);
	UBlueprint* EquipDefBlueprint = FindObject<UBlueprint>(Package, *AssetName);

	if (!EquipDefBlueprint)
	{
		FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");

		const FString PackagePath = FPackageName::GetLongPackagePath(PackageName);
		// Create object and package
		UBlueprintFactory* MyFactory = NewObject<UBlueprintFactory>(UBlueprintFactory::StaticClass());
		MyFactory->bEditAfterNew = true;
		MyFactory->SupportedClass = UBlueprint::StaticClass();
		MyFactory->ParentClass = UOvrlEquipmentDefinition::StaticClass();
		EquipDefBlueprint = Cast<UBlueprint>(AssetToolsModule.Get().CreateAsset(AssetName, PackagePath, UBlueprint::StaticClass(), MyFactory));

		// FSavePackageArgs SavePackageArgs;
		// SavePackageArgs.TopLevelFlags = EObjectFlags::RF_Standalone | EObjectFlags::RF_Public;
		// UPackage::Save(Package, EquipmentBlueprint, *FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension()), SavePackageArgs);
		//
		// // Inform asset registry
		// AssetRegistry.AssetCreated(EquipmentBlueprint);

		FAssetRegistryModule::AssetCreated(EquipDefBlueprint);
		//ReopenObject(EquipDefBlueprint);

		SaveObject(EquipDefBlueprint, PackageName, Package);
	}

	if (!EquipDefBlueprint)
	{
		return nullptr;
	}

	UOvrlEquipmentDefinition* EquipDef = Cast<UOvrlEquipmentDefinition>(EquipDefBlueprint->GeneratedClass->GetDefaultObject());

	if (EquipDef)
	{
		if (EquipmentInstanceBP)
		{
			// EquipDef->Modify();

			EquipDef->bShouldSpawnEquipmentInstance = true;
			EquipDef->EquipmentClass = EquipmentInstanceBP->GeneratedClass;
			EquipDef->AttachSocketName = ItemData.AttachSocketName;

			// EquipDef->ReinitializeProperties();
			//
			// EquipDef->PostEditChange();
			// EquipDef->MarkPackageDirty();
			// SaveObject(EquipDefBlueprint, PackageName, Package);
		}
	}

	FBlueprintEditorUtils::MarkBlueprintAsModified(EquipDefBlueprint);
	FKismetEditorUtilities::CompileBlueprint(EquipDefBlueprint);
	//ReopenObject(EquipDefBlueprint);

	// UPackage* Outer = EquipDefBlueprint->GetOutermost();
	// Outer->MarkPackageDirty();

	return EquipDefBlueprint;
}

UBlueprint* UOvrlInventoryUtils::CreateItemDefinition(const FInventoryItemData& ItemData, const UBlueprint* EquipmentDefinitionBP)
{
	const FString AssetName = "ID_" + ItemData.AssetName;
	const FString PackageName = ItemData.FolderPath + "/" + AssetName;

	UPackage* Package = CreatePackage(*PackageName);
	UBlueprint* ItemDefBlueprint = FindObject<UBlueprint>(Package, *AssetName);

	if (!ItemDefBlueprint)
	{
		FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");

		const FString PackagePath = FPackageName::GetLongPackagePath(PackageName);
		// Create object and package
		UBlueprintFactory* MyFactory = NewObject<UBlueprintFactory>(UBlueprintFactory::StaticClass());
		MyFactory->bEditAfterNew = true;
		MyFactory->SupportedClass = UBlueprint::StaticClass();
		MyFactory->ParentClass = UOvrlItemDefinition::StaticClass();
		ItemDefBlueprint = Cast<UBlueprint>(AssetToolsModule.Get().CreateAsset(AssetName, PackagePath, UBlueprint::StaticClass(), MyFactory));

		// FSavePackageArgs SavePackageArgs;
		// SavePackageArgs.TopLevelFlags = EObjectFlags::RF_Standalone | EObjectFlags::RF_Public;
		// UPackage::Save(Package, EquipmentBlueprint, *FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension()), SavePackageArgs);
		//
		// // Inform asset registry
		// AssetRegistry.AssetCreated(EquipmentBlueprint);

		FAssetRegistryModule::AssetCreated(ItemDefBlueprint);
		//ReopenObject(EquipDefBlueprint);

		SaveObject(ItemDefBlueprint, PackageName, Package);
	}

	if (!ItemDefBlueprint)
	{
		return nullptr;
	}

	UOvrlItemDefinition* ItemDef = Cast<UOvrlItemDefinition>(ItemDefBlueprint->GeneratedClass->GetDefaultObject());

	if (ItemDef)
	{

		ItemDef->DisplayName = ItemData.DisplayName;
		ItemDef->DisplayMesh = ItemData.DisplayMesh;
		ItemDef->DisplayTexture = ItemData.DisplayTexture;

		if (EquipmentDefinitionBP)
		{
			ItemDef->Modify();
			UOvrlItemFragment_EquippableItem* Fragment_EquippableItem = NewObject<UOvrlItemFragment_EquippableItem>(
				ItemDef,
				UOvrlItemFragment_EquippableItem::StaticClass(),
				NAME_None,
				RF_Public | RF_Transactional
			);

			Fragment_EquippableItem->EquipmentDefinition = EquipmentDefinitionBP->GeneratedClass;

			ItemDef->Fragments.Empty();
			ItemDef->Fragments.Add(Fragment_EquippableItem);
			ItemDef->PostEditChange();
		}
	}

	FBlueprintEditorUtils::MarkBlueprintAsModified(ItemDefBlueprint);
	FKismetEditorUtilities::CompileBlueprint(ItemDefBlueprint);

	return ItemDefBlueprint;
}

UPackage* UOvrlInventoryUtils::CreateItemPackage(const FString& FolderPath, const FString& PackageName)
{
	const FString PackagePath = FolderPath + "/" + PackageName; // E.g.: "ED_Rifle"
	return CreatePackage(*PackagePath);
}

void UOvrlInventoryUtils::SaveObject(UObject* Object, const FString& PackagePath, UPackage* Package)
{
	Object->MarkPackageDirty();

	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	FSavePackageArgs SavePackageArgs;
	SavePackageArgs.TopLevelFlags = EObjectFlags::RF_Standalone | EObjectFlags::RF_Public;
	UPackage::Save(Package, Object, *FPackageName::LongPackageNameToFilename(PackagePath, FPackageName::GetAssetPackageExtension()), SavePackageArgs);

	// Inform asset registry
	FAssetRegistryModule::AssetCreated(Object);
	// AssetRegistry.AssetCreated(Object);

	ReopenObject(Object);
}

void UOvrlInventoryUtils::ReopenObject(UObject* Object)
{
	UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
	if (AssetEditorSubsystem)
	{
		// Re-open asset to refresh viewport
		const bool bIsOpen = AssetEditorSubsystem->FindEditorForAsset(Object, false) != nullptr;
		if (bIsOpen)
		{
			AssetEditorSubsystem->CloseAllEditorsForAsset(Object);
			AssetEditorSubsystem->OpenEditorForAsset(Object);

		}
	}
}

// Fill out your copyright notice in the Description page of Project Settings.

#include "EditorUtils/OvrlInventoryEditorUtility.h"

// Internal
#include "Equipment/OvrlEquipmentDefinition.h"
#include "Weapons/OvrlProjectileWeaponInstance.h"
#include "Weapons/AmmoTypes/OvrlItemAmmoBase.h"
#include "Inventory/OvrlItemDefinition.h"
#include "Inventory/OvrlItemFragment_EquippableItem.h"

// Engine
#include "AssetToolsModule.h"
#include "BlueprintEditorLibrary.h"
#include "FileHelpers.h"
#include "OvrlGameplayTags.h"
#include "PackageTools.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Dialog/SMessageDialog.h"
#include "Factories/BlueprintFactory.h"
#include "Factories/DataAssetFactory.h"
#include "Inventory/OvrlItemFragment_SetStats.h"
#include "Inventory/OvrlItemPickupActor.h"
#include "Inventory/OvrlPickupDefinition.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Subsystems/AssetEditorSubsystem.h"

void UOvrlInventoryUtils::CreateItem(UObject* WorldContextObject, const FInventoryItemData& ItemData)
{
	UBlueprint* EquipmentInstanceBP = nullptr;

	if (ItemData.bShouldSpawnInstance)
	{
		EquipmentInstanceBP = CreateEquipmentInstance(ItemData);
	}

	const UBlueprint* EquipmentDefinitionBP = CreateEquipmentDefinition(ItemData, EquipmentInstanceBP);
	const UBlueprint* ItemDefBP = CreateItemDefinition(ItemData, EquipmentDefinitionBP);
	UDataAsset* PickupDefinitionDA = CreatePickupDefinition(ItemData, ItemDefBP);
	UBlueprint* PickupActor = CreatePickupActor(ItemData, PickupDefinitionDA);
}

UBlueprint* UOvrlInventoryUtils::CreateEquipmentInstance(const FInventoryItemData& ItemData)
{
	if (!ItemData.EquipmentInstanceClass)
	{
		return nullptr;
	}

	UBlueprint* EquipmentBlueprint = FindOrCreateBlueprint(ItemData, "BP_", ItemData.EquipmentInstanceClass);

	if (!EquipmentBlueprint)
	{
		return nullptr;
	}

	UBlueprintEditorLibrary::ReparentBlueprint(EquipmentBlueprint, ItemData.EquipmentInstanceClass);

	UObject* CDO = EquipmentBlueprint->GeneratedClass->GetDefaultObject();

	if (AOvrlWeaponInstance* WeaponInstance = Cast<AOvrlWeaponInstance>(CDO))
	{
		if (WeaponInstance->WeaponMesh)
		{
			WeaponInstance->WeaponMesh->SetSkeletalMesh(ItemData.WeaponMesh);
		}

		WeaponInstance->BaseDamage = ItemData.BaseDamage;
		WeaponInstance->GE_Damage = ItemData.GE_DamageClass;
	}

	if (AOvrlRangedWeaponInstance* RangedWeaponInstance = Cast<AOvrlRangedWeaponInstance>(CDO))
	{
		if (ItemData.AmmoType)
		{
			RangedWeaponInstance->AmmoType = ItemData.AmmoType;
		}
		else
		{
			RangedWeaponInstance->AmmoType = UOvrlItemAmmoBase::StaticClass();
		}

		RangedWeaponInstance->BulletsPerCartridge = ItemData.BulletsPerCartridge;
		RangedWeaponInstance->FireRate = ItemData.FireRate;
	}

	if (AOvrlProjectileWeaponInstance* ProjectileWeaponInstance = Cast<AOvrlProjectileWeaponInstance>(CDO))
	{
		ProjectileWeaponInstance->ProjectileClass = ItemData.ProjectileClass;
	}

	FBlueprintEditorUtils::MarkBlueprintAsModified(EquipmentBlueprint);
	FKismetEditorUtilities::CompileBlueprint(EquipmentBlueprint);
	UEditorLoadingAndSavingUtils::SavePackages({ EquipmentBlueprint->GetPackage() }, false);

	ReopenObject(EquipmentBlueprint);

	return EquipmentBlueprint;
}

UBlueprint* UOvrlInventoryUtils::CreateEquipmentDefinition(const FInventoryItemData& ItemData, const UBlueprint* EquipmentInstanceBP)
{
	UBlueprint* EquipDefBlueprint = FindOrCreateBlueprint(ItemData, "ED_", UOvrlEquipmentDefinition::StaticClass());

	if (!EquipDefBlueprint)
	{
		return nullptr;
	}

	UOvrlEquipmentDefinition* EquipDef = Cast<UOvrlEquipmentDefinition>(EquipDefBlueprint->GeneratedClass->GetDefaultObject());

	if (EquipDef)
	{
		EquipDef->AbilitySetsToGrant.Empty();
		EquipDef->AbilitySetsToGrant.Add(ItemData.AbilitySet);

		EquipDef->bAllowQuickSlot = ItemData.bAllowQuickSlot;
		EquipDef->bShouldSpawnEquipmentInstance = ItemData.bShouldSpawnInstance;

		if (ItemData.bShouldSpawnInstance && EquipmentInstanceBP)
		{
			EquipDef->EquipmentClass = EquipmentInstanceBP->GeneratedClass;
			EquipDef->AttachSocketName = ItemData.AttachSocketName;
		}
	}

	FBlueprintEditorUtils::MarkBlueprintAsModified(EquipDefBlueprint);
	FKismetEditorUtilities::CompileBlueprint(EquipDefBlueprint);
	UEditorLoadingAndSavingUtils::SavePackages({ EquipDefBlueprint->GetPackage() }, false);

	return EquipDefBlueprint;
}

UBlueprint* UOvrlInventoryUtils::CreateItemDefinition(const FInventoryItemData& ItemData, const UBlueprint* EquipmentDefinitionBP)
{
	UBlueprint* ItemDefBlueprint = FindOrCreateBlueprint(ItemData, "ID_", UOvrlItemDefinition::StaticClass());

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

			if (UOvrlEquipmentDefinition* EquipmentDefinitionCDO = Cast<UOvrlEquipmentDefinition>(EquipmentDefinitionBP->GeneratedClass->GetDefaultObject()))
			{
				if (EquipmentDefinitionCDO->EquipmentClass->IsChildOf<AOvrlWeaponInstance>())
				{
					UOvrlItemFragment_SetStats* Fragment_SetStats = NewObject<UOvrlItemFragment_SetStats>(
						ItemDef,
						UOvrlItemFragment_SetStats::StaticClass(),
						NAME_None,
						RF_Public | RF_Transactional
					);

					// Initialize starting ammo
					Fragment_SetStats->InitialItemStats.Add(OvrlWeaponTags::MagazineSize, ItemData.MagazineSize);
					Fragment_SetStats->InitialItemStats.Add(OvrlWeaponTags::MagazineAmmo, ItemData.MagazineSize);
					ItemDef->Fragments.Add(Fragment_SetStats);
				}
			}

			ItemDef->PostEditChange();
		}
	}

	FBlueprintEditorUtils::MarkBlueprintAsModified(ItemDefBlueprint);
	FKismetEditorUtilities::CompileBlueprint(ItemDefBlueprint);
	UEditorLoadingAndSavingUtils::SavePackages({ ItemDefBlueprint->GetPackage() }, false);

	return ItemDefBlueprint;
}

UDataAsset* UOvrlInventoryUtils::CreatePickupDefinition(const FInventoryItemData& ItemData, const UBlueprint* ItemDefinitionBP)
{
	UDataAsset* PickupDefinitionDA = FindOrCreateDataAsset(ItemData, "PickupDefinition_", UOvrlPickupDefinition::StaticClass());

	if (!PickupDefinitionDA)
	{
		return nullptr;
	}

	UOvrlPickupDefinition* PickupDef = Cast<UOvrlPickupDefinition>(PickupDefinitionDA);

	if (PickupDef && ItemDefinitionBP)
	{
		PickupDef->ItemDefinition = ItemDefinitionBP->GeneratedClass;
	}

	UEditorLoadingAndSavingUtils::SavePackages({ PickupDefinitionDA->GetPackage() }, false);

	return PickupDefinitionDA;
}

UBlueprint* UOvrlInventoryUtils::CreatePickupActor(const FInventoryItemData& ItemData, UDataAsset* PickupDefinitionDA)
{
	UBlueprint* PickupActorBlueprint = FindOrCreateBlueprint(ItemData, "BP_ItemPickupActor_", ItemData.PickupActorClass);

	if (!PickupActorBlueprint)
	{
		return nullptr;
	}

	AOvrlItemPickupActor* PickupActor = Cast<AOvrlItemPickupActor>(PickupActorBlueprint->GeneratedClass->GetDefaultObject());

	if (PickupActor)
	{
		PickupActor->ItemPickupDefinition = Cast<UOvrlPickupDefinition>(PickupDefinitionDA);
		PickupActor->ItemMesh->SetStaticMesh(ItemData.DisplayMesh);
	}

	FBlueprintEditorUtils::MarkBlueprintAsModified(PickupActorBlueprint);
	FKismetEditorUtilities::CompileBlueprint(PickupActorBlueprint);
	UEditorLoadingAndSavingUtils::SavePackages({ PickupActorBlueprint->GetPackage() }, false);

	return PickupActorBlueprint;
}

UBlueprint* UOvrlInventoryUtils::FindOrCreateBlueprint(const FInventoryItemData& ItemData, const FString& Prefix, TSubclassOf<UObject> ParentClass)
{
	const FString AssetName = Prefix + ItemData.AssetName;
	const FString PackageName = ItemData.FolderPath + "/" + AssetName;

	UPackage* Package = CreatePackage(*PackageName);
	if (!UPackageTools::HandleFullyLoadingPackages({ Package }, FText::FromString("Create a new object")))
	{
		// User aborted.
		return nullptr;
	}

	UBlueprint* Blueprint = FindObject<UBlueprint>(Package, *AssetName);

	if (!Blueprint)
	{
		FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");

		const FString PackagePath = FPackageName::GetLongPackagePath(PackageName);
		// Create object and package
		UBlueprintFactory* MyFactory = NewObject<UBlueprintFactory>(UBlueprintFactory::StaticClass());
		MyFactory->bEditAfterNew = true;
		MyFactory->SupportedClass = UBlueprint::StaticClass();
		MyFactory->ParentClass = ParentClass;
		Blueprint = Cast<UBlueprint>(AssetToolsModule.Get().CreateAsset(AssetName, PackagePath, UBlueprint::StaticClass(), MyFactory));

		FAssetRegistryModule::AssetCreated(Blueprint);
	}
	else if (!ItemData.bAlwaysOverwrite)
	{
		// Object already exists in either the specified package or another package.
		// Check to see if the user wants to replace the object.
		TSharedRef<SMessageDialog> ConfirmDialog = SNew(SMessageDialog)
			.Icon(FAppStyle::Get().GetBrush("Icons.WarningWithColor.Large"))
			.Title(FText(NSLOCTEXT("OverlinkEd", "ReplaceExistingObjectInPackageConfirmation_Title", "Overwrite Existing Object")))
			.Message(FText::Format(NSLOCTEXT("OverlinkEd", "ReplaceExistingObjectInPackageConfirmation_Message", "An object already exists with this name.\n\n\tName: {0}\n\tClass: {1}\n\tAsset path: {2}"
					"\n\nOverwriting will replace all matching properties of the existing object with the values from the creation tool.\n\nOverwrite the existing object?"),
				FText::FromString(AssetName),
				FText::FromString(Blueprint->GetClass()->GetName()),
				FText::FromString(PackageName)))
			.Buttons({
				SCustomDialog::FButton(NSLOCTEXT("OverlinkEd", "ReplaceExistingObjectInPackageConfirmation_ButtonOverwrite", "Overwrite")).SetPrimary(true),
				SCustomDialog::FButton(NSLOCTEXT("OverlinkEd", "ReplaceExistingObjectInPackageConfirmation_ButtonCancel", "Cancel")),
			})
			.ContentMinWidth(300.0f);
		uint32 ConfirmationResult = ConfirmDialog->ShowModal();

		const bool bWantReplace = ConfirmationResult == 0;

		if (!bWantReplace)
		{
			return nullptr;
		}
	}

	return Blueprint;
}

UDataAsset* UOvrlInventoryUtils::FindOrCreateDataAsset(const FInventoryItemData& ItemData, const FString& Prefix, TSubclassOf<UObject> ParentClass)
{
	const FString AssetName = Prefix + ItemData.AssetName;
	const FString PackageName = ItemData.FolderPath + "/" + AssetName;

	UPackage* Package = CreatePackage(*PackageName);
	if (!UPackageTools::HandleFullyLoadingPackages({ Package }, FText::FromString("Create a new object")))
	{
		// User aborted.
		return nullptr;
	}

	UDataAsset* Blueprint = FindObject<UDataAsset>(Package, *AssetName);

	if (!Blueprint)
	{
		FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");

		const FString PackagePath = FPackageName::GetLongPackagePath(PackageName);
		// Create object and package
		UDataAssetFactory* MyFactory = NewObject<UDataAssetFactory>(UDataAssetFactory::StaticClass());
		MyFactory->bEditAfterNew = true;
		MyFactory->SupportedClass = UDataAsset::StaticClass();
		MyFactory->DataAssetClass = ParentClass;
		Blueprint = Cast<UDataAsset>(AssetToolsModule.Get().CreateAsset(AssetName, PackagePath, UDataAsset::StaticClass(), MyFactory));

		FAssetRegistryModule::AssetCreated(Blueprint);
	}
	else if (!ItemData.bAlwaysOverwrite)
	{
		// Object already exists in either the specified package or another package.
		// Check to see if the user wants to replace the object.
		TSharedRef<SMessageDialog> ConfirmDialog = SNew(SMessageDialog)
			.Icon(FAppStyle::Get().GetBrush("Icons.WarningWithColor.Large"))
			.Title(FText(NSLOCTEXT("OverlinkEd", "ReplaceExistingObjectInPackageConfirmation_Title", "Overwrite Existing Object")))
			.Message(FText::Format(NSLOCTEXT("OverlinkEd", "ReplaceExistingObjectInPackageConfirmation_Message", "An object already exists with this name.\n\n\tName: {0}\n\tClass: {1}\n\tAsset path: {2}"
					"\n\nOverwriting will replace all matching properties of the existing object with the values from the creation tool.\n\nOverwrite the existing object?"),
				FText::FromString(AssetName),
				FText::FromString(Blueprint->GetClass()->GetName()),
				FText::FromString(PackageName)))
			.Buttons({
				SCustomDialog::FButton(NSLOCTEXT("OverlinkEd", "ReplaceExistingObjectInPackageConfirmation_ButtonOverwrite", "Overwrite")).SetPrimary(true),
				SCustomDialog::FButton(NSLOCTEXT("OverlinkEd", "ReplaceExistingObjectInPackageConfirmation_ButtonCancel", "Cancel")),
			})
			.ContentMinWidth(300.0f);
		uint32 ConfirmationResult = ConfirmDialog->ShowModal();

		const bool bWantReplace = ConfirmationResult == 0;

		if (!bWantReplace)
		{
			return nullptr;
		}
	}

	return Blueprint;
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

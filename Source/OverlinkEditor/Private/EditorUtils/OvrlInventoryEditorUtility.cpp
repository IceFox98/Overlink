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
#include "ContentBrowserModule.h"
#include "FileHelpers.h"
#include "IContentBrowserSingleton.h"
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

FInventoryItemData UOvrlInventoryUtils::ItemDataRef;

void UOvrlInventoryUtils::CreateItem(UObject* WorldContextObject, const FInventoryItemData& ItemData)
{
	if (ItemData.AssetName.TrimStartAndEnd().IsEmpty())
	{
		ShowMessageDialog("Invalid Asset Name", FText::FromString("You have to provide at least 1 character for Asset Name field!"), "Ok");
		return;
	}

	// Use static to avoid memory access violation
	ItemDataRef = ItemData;

	// Create folder selection window
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");

	FPathPickerConfig PathPickerConfig;
	PathPickerConfig.bAllowReadOnlyFolders = false;
	PathPickerConfig.bForceShowEngineContent = false;
	PathPickerConfig.OnPathSelected = FOnPathSelected::CreateLambda([](const FString& SelectedPath) {
		UE_LOG(LogTemp, Warning, TEXT("%s"), *SelectedPath);
		ItemDataRef.FolderPath = SelectedPath;
	});

	TSharedRef<SWindow> Window = FSlateApplication::Get().AddWindow(
		SNew(SWindow)
		.Title(FText::FromString("Select the destination folder for the items"))
		.SupportsMaximize(false)
		.SupportsMinimize(false)
		.IsTopmostWindow(true)
		.ClientSize(FVector2D(400, 500))
		.AutoCenter(EAutoCenter::PreferredWorkArea)
	);

	TWeakPtr<SWindow> WeakWindow = Window;

	Window->SetContent(
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.FillHeight(1.0f)
		[
			ContentBrowserModule.Get().CreatePathPicker(PathPickerConfig)
		]
		+ SVerticalBox::Slot()
		.HAlign(HAlign_Right)
		.Padding(0, 20, 0, 0)
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.Padding(0, 2, 6, 0)
			.AutoWidth()
			[
				SNew(SButton)
				.VAlign(VAlign_Bottom)
				.ContentPadding(FMargin(8, 2, 8, 2))
				.OnClicked(FOnClicked::CreateLambda([WeakWindow, WorldContextObject]() {
					if (WeakWindow.IsValid())
					{
						WeakWindow.Pin()->RequestDestroyWindow();
					}

					UOvrlInventoryUtils::CreateItemInternal(WorldContextObject, ItemDataRef);

					return FReply::Handled();
				}))
				.ButtonStyle(FAppStyle::Get(), "FlatButton.Success")
				.TextStyle(FAppStyle::Get(), "FlatButton.DefaultTextStyle")
				.Text(FText::FromString("Create"))
			]
			+ SHorizontalBox::Slot()
			.Padding(0, 2, 0, 0)
			.AutoWidth()
			[
				SNew(SButton)
				.VAlign(VAlign_Bottom)
				.ContentPadding(FMargin(8, 2, 8, 2))
				.OnClicked(FOnClicked::CreateLambda([WeakWindow]() {
					if (WeakWindow.IsValid())
					{
						WeakWindow.Pin()->RequestDestroyWindow();
					}
					return FReply::Handled();
				}))
				.ButtonStyle(FAppStyle::Get(), "FlatButton.Default")
				.TextStyle(FAppStyle::Get(), "FlatButton.DefaultTextStyle")
				.Text(FText::FromString("Cancel"))
			]
		]
	);
}

void UOvrlInventoryUtils::CreateItemInternal(UObject* WorldContextObject, const FInventoryItemData& ItemData)
{
	const UBlueprint* EquipmentInstanceBP = CreateEquipmentInstance(ItemData);;
	const UBlueprint* EquipmentDefinitionBP = CreateEquipmentDefinition(ItemData, EquipmentInstanceBP);
	const UBlueprint* ItemDefBP = CreateItemDefinition(ItemData, EquipmentDefinitionBP);
	UDataAsset* PickupDefinitionDA = CreatePickupDefinition(ItemData, ItemDefBP);
	UBlueprint* PickupActor = CreatePickupActor(ItemData, PickupDefinitionDA);

	// Navigate to the created asset.
	if (ItemDefBP)
	{
		const TArray<FAssetData>& Assets = { ItemDefBP };
		const FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
		ContentBrowserModule.Get().SyncBrowserToAssets(Assets);
	}
}

UBlueprint* UOvrlInventoryUtils::CreateEquipmentInstance(const FInventoryItemData& ItemData)
{
	if (!ItemData.bIsEquippable || !ItemData.bShouldSpawnInstance)
	{
		return nullptr;
	}

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
	if (!ItemData.bIsEquippable)
	{
		return nullptr;
	}

	UBlueprint* EquipDefBlueprint = FindOrCreateBlueprint(ItemData, "ED_", UOvrlEquipmentDefinition::StaticClass());

	if (!EquipDefBlueprint)
	{
		return nullptr;
	}

	UOvrlEquipmentDefinition* EquipDef = Cast<UOvrlEquipmentDefinition>(EquipDefBlueprint->GeneratedClass->GetDefaultObject());

	if (EquipDef)
	{
		EquipDef->AbilitySetsToGrantToItem.Empty();
		EquipDef->AbilitySetsToGrantToItem.Add(ItemData.AbilitySet);

		EquipDef->bSetAsActiveSlotOnAdded = ItemData.bAllowQuickSlot;
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

		if (ItemData.bIsEquippable && EquipmentDefinitionBP)
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
				if (EquipmentDefinitionCDO->EquipmentClass && EquipmentDefinitionCDO->EquipmentClass->IsChildOf<AOvrlWeaponInstance>())
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
		TSharedRef<SMessageDialog> ConfirmDialog = ShowDuplicatedAssetDialog(AssetName, PackageName, Blueprint);
		const uint32 ConfirmationResult = ConfirmDialog->ShowModal();
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

	UDataAsset* DataAsset = FindObject<UDataAsset>(Package, *AssetName);

	if (!DataAsset)
	{
		FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");

		const FString PackagePath = FPackageName::GetLongPackagePath(PackageName);
		// Create object and package
		UDataAssetFactory* MyFactory = NewObject<UDataAssetFactory>(UDataAssetFactory::StaticClass());
		MyFactory->bEditAfterNew = true;
		MyFactory->SupportedClass = UDataAsset::StaticClass();
		MyFactory->DataAssetClass = ParentClass;
		DataAsset = Cast<UDataAsset>(AssetToolsModule.Get().CreateAsset(AssetName, PackagePath, UDataAsset::StaticClass(), MyFactory));

		FAssetRegistryModule::AssetCreated(DataAsset);
	}
	else if (!ItemData.bAlwaysOverwrite)
	{
		// Object already exists in either the specified package or another package.
		// Check to see if the user wants to replace the object.
		TSharedRef<SMessageDialog> ConfirmDialog = ShowDuplicatedAssetDialog(AssetName, PackageName, DataAsset);
		const uint32 ConfirmationResult = ConfirmDialog->ShowModal();
		const bool bWantReplace = ConfirmationResult == 0;

		if (!bWantReplace)
		{
			return nullptr;
		}
	}

	return DataAsset;
}

TSharedRef<SMessageDialog> UOvrlInventoryUtils::ShowDuplicatedAssetDialog(const FString& AssetName, const FString& PackageName, const UObject* Asset)
{
	if (!Asset)
	{
		return ShowMessageDialog("Invalid Asset", FText::FromString("Can't show dialog due to invalid Asset"), "Ok");
	}

	FText Message = FText::Format(FText::FromString("An object already exists with this name.\n\n\tName: {0}\n\tClass: {1}\n\tAsset path: {2}"
			"\n\nOverwriting will replace all matching properties of the existing object with the values from the creation tool.\n\nOverwrite the existing object?"),
		FText::FromString(AssetName),
		FText::FromString(Asset->GetClass()->GetName()),
		FText::FromString(PackageName));

	return ShowMessageDialog("Overwrite Existing Object", Message, "Overwrite", true);
}

TSharedRef<SMessageDialog> UOvrlInventoryUtils::ShowMessageDialog(const FString& Title, const FText& Message, const FString& SubmitButtonText, bool bShowCancelButton /* = false*/)
{
	TArray<SCustomDialog::FButton> Buttons;
	Buttons.Add(SCustomDialog::FButton(FText::FromString(SubmitButtonText)).SetPrimary(true));
	if (bShowCancelButton)
	{
		Buttons.Add(SCustomDialog::FButton(FText::FromString("Cancel")));
	}

	return SNew(SMessageDialog)
		.Icon(FAppStyle::Get().GetBrush("Icons.WarningWithColor.Large"))
		.Title(FText::FromString(Title))
		.Message(Message)
		.Buttons(Buttons)
		.ContentMinWidth(300.0f);
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

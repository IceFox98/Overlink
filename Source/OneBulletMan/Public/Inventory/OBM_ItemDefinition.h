// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "OBM_ItemDefinition.generated.h"

class UOBM_ItemInstance;

// Represents a fragment of an item definition
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class ONEBULLETMAN_API UOBM_ItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UOBM_ItemInstance* Instance) const {}
};

/**
 * 
 */
UCLASS(Blueprintable, Abstract, EditInlineNew)
class ONEBULLETMAN_API UOBM_ItemDefinition : public UObject
{
	GENERATED_BODY()

public:
	UOBM_ItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:

	// Search for the passed class through all the Fragments of this item
	const UOBM_ItemFragment* FindFragmentByClass(TSubclassOf<UOBM_ItemFragment> FragmentClass) const;

public:

	// The display name for this item in the inventory
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
		FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Display, Instanced)
		TArray<TObjectPtr<UOBM_ItemFragment>> Fragments;
};

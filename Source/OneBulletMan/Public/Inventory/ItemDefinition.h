// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemDefinition.generated.h"

class UItemInstance;

// Represents a fragment of an item definition
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class ONEBULLETMAN_API UItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UItemInstance* Instance) const {}
};

/**
 * 
 */
UCLASS(Blueprintable, Abstract, EditInlineNew)
class ONEBULLETMAN_API UItemDefinition : public UObject
{
	GENERATED_BODY()

public:
	UItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:

	// Search for the passed class through all the Fragments of this item
	const UItemFragment* FindFragmentByClass(TSubclassOf<UItemFragment> FragmentClass) const;

public:

	// The display name for this item in the inventory
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
		FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Display, Instanced)
		TArray<TObjectPtr<UItemFragment>> Fragments;
};

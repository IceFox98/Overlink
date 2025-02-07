// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "OvrlItemDefinition.generated.h"

class UOvrlItemInstance;

// Represents a fragment of an item definition
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class OVERLINK_API UOvrlItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UOvrlItemInstance* Instance) const {}
};

/**
 * 
 */
UCLASS(Blueprintable, Abstract, EditInlineNew)
class OVERLINK_API UOvrlItemDefinition : public UObject
{
	GENERATED_BODY()

public:
	UOvrlItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:

	// Search for the passed class through all the Fragments of this item
	const UOvrlItemFragment* FindFragmentByClass(TSubclassOf<UOvrlItemFragment> FragmentClass) const;

public:

	// The display name for this item in the inventory
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
		FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Display, Instanced)
		TArray<TObjectPtr<UOvrlItemFragment>> Fragments;
};

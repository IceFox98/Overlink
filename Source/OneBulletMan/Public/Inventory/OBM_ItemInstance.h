// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"

#include "OBM_ItemInstance.generated.h"

class UOBM_ItemDefinition;
class UOBM_ItemFragment;

/**
 * Represents one stack of a gameplay tag (tag + count)
 */
USTRUCT(BlueprintType)
struct FGameplayTagStack
{
	GENERATED_BODY()

public:

	FGameplayTagStack() {}

	FGameplayTagStack(FGameplayTag InTag, int32 InStackCount)
		: Tag(InTag)
		, StackCount(InStackCount)
	{}

private:

	friend class UOBM_ItemInstance;

	UPROPERTY()
		FGameplayTag Tag;

	UPROPERTY()
		int32 StackCount = 0;
};

/**
 *
 */
UCLASS(BlueprintType)
class ONEBULLETMAN_API UOBM_ItemInstance : public UObject
{
	GENERATED_BODY()

public:
	// Adds a specified number of stacks to the tag (does nothing if StackCount is below 1)
	void AddStack(FGameplayTag Tag, int32 StackCount);

	// Removes a specified number of stacks from the tag (does nothing if StackCount is below 1)
	void RemoveStack(FGameplayTag Tag, int32 StackCount);

	int32 GetTagStackCount(FGameplayTag Tag) const;

	void SetItemDef(TSubclassOf<UOBM_ItemDefinition> InDef);

	FORCEINLINE TSubclassOf<UOBM_ItemDefinition> GetItemDef() const { return ItemDef; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, meta = (DeterminesOutputType = FragmentClass))
		const UOBM_ItemFragment* FindFragmentByClass(TSubclassOf<UOBM_ItemFragment> FragmentClass) const;

	template <typename ResultClass>
	const ResultClass* FindFragmentByClass() const
	{
		return (ResultClass*)FindFragmentByClass(ResultClass::StaticClass());
	}

private:

	TSubclassOf<UOBM_ItemDefinition> ItemDef;

	// List of gameplay tag stacks
	UPROPERTY()
		TArray<FGameplayTagStack> Stacks;

	// Accelerated list of tag stacks for queries
	UPROPERTY()
		TMap<FGameplayTag, int32> TagToCountMap;

};

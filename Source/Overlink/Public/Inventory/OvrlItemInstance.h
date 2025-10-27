// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"

#include "OvrlItemInstance.generated.h"

class UOvrlItemDefinition;
class UOvrlItemFragment;

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

	friend class UOvrlItemInstance;

	UPROPERTY(VisibleInstanceOnly)
		FGameplayTag Tag;

	UPROPERTY(VisibleInstanceOnly)
		int32 StackCount = 0;
};

/**
 *
 */
UCLASS(BlueprintType)
class OVERLINK_API UOvrlItemInstance : public UObject
{
	GENERATED_BODY()

public:
	// Adds a specified number of stacks to the tag (does nothing if StackCount is below 1)
	void AddStack(FGameplayTag Tag, int32 StackCount);

	// Removes a specified number of stacks from the tag (does nothing if StackCount is below 1)
	void RemoveStack(FGameplayTag Tag, int32 StackCount);

	int32 GetTagStackCount(FGameplayTag Tag) const;

	void SetItemDef(const TSubclassOf<UOvrlItemDefinition>& InItemDef);

	FORCEINLINE TSubclassOf<UOvrlItemDefinition> GetItemDef() const { return ItemDef; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, meta = (DeterminesOutputType = FragmentClass))
		const UOvrlItemFragment* FindFragmentByClass(TSubclassOf<UOvrlItemFragment> FragmentClass) const;

	template <typename ResultClass>
	const ResultClass* FindFragmentByClass() const
	{
		return (ResultClass*)FindFragmentByClass(ResultClass::StaticClass());
	}

private:

	TSubclassOf<UOvrlItemDefinition> ItemDef;

	// List of gameplay tag stacks
	UPROPERTY(VisibleInstanceOnly)
		TArray<FGameplayTagStack> Stacks;

	// Accelerated list of tag stacks for queries
	UPROPERTY()
		TMap<FGameplayTag, int32> TagToCountMap;

};

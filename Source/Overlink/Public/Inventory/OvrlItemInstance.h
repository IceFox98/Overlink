// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/OvrlAbilitySet.h"
#include "GameplayTagContainer.h"

#include "OvrlItemInstance.generated.h"

class UOvrlPickupDefinition;
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
	FGameplayTagStack()
	{
	}

	FGameplayTagStack(FGameplayTag InTag, int32 InStackCount)
		: Tag(InTag)
		  , StackCount(InStackCount)
	{
	}

private:
	friend class UOvrlItemInstance;

	UPROPERTY(VisibleInstanceOnly, SaveGame)
	FGameplayTag Tag;

	UPROPERTY(VisibleInstanceOnly, SaveGame)
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
	UFUNCTION(BlueprintCallable, Category = "Ovrl Item Instance")
	void AddStack(FGameplayTag Tag, int32 StackCount);

	// Removes a specified number of stacks from the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, Category = "Ovrl Item Instance")
	void RemoveStack(FGameplayTag Tag, int32 StackCount);

	UFUNCTION(BlueprintCallable, Category = "Ovrl Item Instance")
	void ReplaceStacks(const TArray<FGameplayTagStack>& InStacks);

	UFUNCTION(BlueprintCallable, Category = "Ovrl Item Instance")
	int32 GetTagStackCount(FGameplayTag Tag) const;

	void SetItemDef(const TSubclassOf<UOvrlItemDefinition>& InItemDefClass) { ItemDefClass = InItemDefClass; }
	FORCEINLINE TSubclassOf<UOvrlItemDefinition> GetItemDefClass() const { return ItemDefClass; }

	UFUNCTION(BlueprintCallable, Category = "Ovrl Item Instance")
	UOvrlItemDefinition* GetItemDef() const;

	FORCEINLINE FOvrlAbilitySet_GrantedHandles& GetGrantedHandles() { return GrantedHandles; }
	FORCEINLINE const TArray<FGameplayTagStack>& GetStacks() { return Stacks; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Ovrl Item Instance", meta = (DeterminesOutputType = FragmentClass))
	const UOvrlItemFragment* FindFragmentByClass(TSubclassOf<UOvrlItemFragment> FragmentClass) const;

	template <typename ResultClass>
	const ResultClass* FindFragmentByClass() const
	{
		return (ResultClass*)FindFragmentByClass(ResultClass::StaticClass());
	}

public:
	FGuid Guid;

private:
	TSubclassOf<UOvrlItemDefinition> ItemDefClass;

	// List of gameplay tag stacks
	UPROPERTY(VisibleInstanceOnly, Category = "Ovrl Item Instance")
	TArray<FGameplayTagStack> Stacks;

	// Accelerated list of tag stacks for queries
	TMap<FGameplayTag, int32> TagToCountMap;

	// List of granted handles
	FOvrlAbilitySet_GrantedHandles GrantedHandles;
};

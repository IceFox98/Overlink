// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/ItemInstance.h"
#include "Inventory/ItemDefinition.h"

void UItemInstance::AddStack(FGameplayTag Tag, int32 StackCount)
{
	if (!Tag.IsValid())
	{
		FFrame::KismetExecutionMessage(TEXT("An invalid tag was passed to AddStack"), ELogVerbosity::Warning);
		return;
	}

	if (StackCount > 0)
	{
		for (FGameplayTagStack& Stack : Stacks)
		{
			if (Stack.Tag == Tag)
			{
				const int32 NewCount = Stack.StackCount + StackCount;
				Stack.StackCount = NewCount;
				TagToCountMap[Tag] = NewCount;
				return;
			}
		}

		Stacks.Emplace(Tag, StackCount);
		TagToCountMap.Add(Tag, StackCount);
	}
}

void UItemInstance::RemoveStack(FGameplayTag Tag, int32 StackCount)
{
	if (!Tag.IsValid())
	{
		FFrame::KismetExecutionMessage(TEXT("An invalid tag was passed to RemoveStack"), ELogVerbosity::Warning);
		return;
	}

	if (StackCount > 0)
	{
		for (auto It = Stacks.CreateIterator(); It; ++It)
		{
			FGameplayTagStack& Stack = *It;
			if (Stack.Tag == Tag)
			{
				if (Stack.StackCount <= StackCount)
				{
					It.RemoveCurrent();
					TagToCountMap.Remove(Tag);
				}
				else
				{
					const int32 NewCount = Stack.StackCount - StackCount;
					Stack.StackCount = NewCount;
					TagToCountMap[Tag] = NewCount;
				}
				return;
			}
		}
	}
}

int32 UItemInstance::GetTagStackCount(FGameplayTag Tag) const
{
	if (TagToCountMap.IsEmpty() || TagToCountMap.Num() <= 0)
	{
		return 0;
	}

	return TagToCountMap.FindRef(Tag);
}

void UItemInstance::SetItemDef(TSubclassOf<UItemDefinition> InDef)
{
	ItemDef = InDef;
}

const UItemFragment* UItemInstance::FindFragmentByClass(TSubclassOf<UItemFragment> FragmentClass) const
{
	if ((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		return GetDefault<UItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}

	return nullptr;
}

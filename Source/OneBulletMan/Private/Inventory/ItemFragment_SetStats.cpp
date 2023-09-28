// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/ItemFragment_SetStats.h"
#include "Inventory/ItemInstance.h"
#include "GameplayTagContainer.h"

void UItemFragment_SetStats::OnInstanceCreated(UItemInstance* Instance) const
{
	for (const auto& KVP : InitialItemStats)
	{
		Instance->AddStack(KVP.Key, KVP.Value);
	}
}

int32 UItemFragment_SetStats::GetItemStatByTag(FGameplayTag Tag) const
{
	if (const int32* StatPtr = InitialItemStats.Find(Tag))
	{
		return *StatPtr;
	}

	return 0;
}

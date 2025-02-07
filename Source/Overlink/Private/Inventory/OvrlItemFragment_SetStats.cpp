// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/OvrlItemFragment_SetStats.h"
#include "Inventory/OvrlItemInstance.h"
#include "GameplayTagContainer.h"

void UOvrlItemFragment_SetStats::OnInstanceCreated(UOvrlItemInstance* Instance) const
{
	for (const auto& KVP : InitialItemStats)
	{
		Instance->AddStack(KVP.Key, KVP.Value);
	}
}

int32 UOvrlItemFragment_SetStats::GetItemStatByTag(FGameplayTag Tag) const
{
	if (const int32* StatPtr = InitialItemStats.Find(Tag))
	{
		return *StatPtr;
	}

	return 0;
}

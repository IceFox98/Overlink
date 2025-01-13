// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/OBM_ItemFragment_SetStats.h"
#include "Inventory/OBM_ItemInstance.h"
#include "GameplayTagContainer.h"

void UOBM_ItemFragment_SetStats::OnInstanceCreated(UOBM_ItemInstance* Instance) const
{
	for (const auto& KVP : InitialItemStats)
	{
		Instance->AddStack(KVP.Key, KVP.Value);
	}
}

int32 UOBM_ItemFragment_SetStats::GetItemStatByTag(FGameplayTag Tag) const
{
	if (const int32* StatPtr = InitialItemStats.Find(Tag))
	{
		return *StatPtr;
	}

	return 0;
}

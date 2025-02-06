// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/OBM_ItemDefinition.h"

UOBM_ItemDefinition::UOBM_ItemDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UOBM_ItemFragment* UOBM_ItemDefinition::FindFragmentByClass(TSubclassOf<UOBM_ItemFragment> FragmentClass) const
{
	if (FragmentClass != nullptr)
	{
		for (UOBM_ItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}

	return nullptr;
}

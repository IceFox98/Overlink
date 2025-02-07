// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/OvrlItemDefinition.h"

UOvrlItemDefinition::UOvrlItemDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UOvrlItemFragment* UOvrlItemDefinition::FindFragmentByClass(TSubclassOf<UOvrlItemFragment> FragmentClass) const
{
	if (FragmentClass != nullptr)
	{
		for (UOvrlItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}

	return nullptr;
}

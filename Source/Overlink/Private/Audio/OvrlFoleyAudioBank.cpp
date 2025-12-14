// Fill out your copyright notice in the Description page of Project Settings.


#include "Audio/OvrlFoleyAudioBank.h"

USoundBase* UOvrlFoleyAudioBank::GetSound(const FGameplayTag& Event, EPhysicalSurface SurfaceType) const
{
	const FSurfaceSounds* Asset = Assets.Find(Event);
	if (Asset)
	{
		return Asset->SurfaceSounds.FindRef(SurfaceType);
	}

	return nullptr;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Audio/OvrlFoleyAudioBank.h"

#include "OvrlUtils.h"
#include "Overlink.h"

USoundBase* UOvrlFoleyAudioBank::GetSound(const FGameplayTag& Event, EPhysicalSurface SurfaceType) const
{
	const FSurfaceSounds* Asset = Assets.Find(Event);
	if (Asset)
	{
		return Asset->SurfaceSounds.FindRef(SurfaceType).LoadSynchronous();
	}

	OVRL_LOG_ERR(LogOverlink, true, "Unable to find Sound for event tag %s", *Event.ToString());
	return nullptr;
}

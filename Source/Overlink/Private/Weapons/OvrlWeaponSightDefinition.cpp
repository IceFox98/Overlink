// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/OvrlWeaponSightDefinition.h"

float UOvrlWeaponSightDefinition::GetMagnifiedFOV(float InFOV)
{
	float TargetMagnification = 1.f;

	switch (SightMagnification)
	{
	case ESightMagnification::One:
		TargetMagnification = 1.1f; // We still want a little bit of FOV when using 1x scope
		break;
	case ESightMagnification::OneHalf:
		TargetMagnification = 1.5f;
		break;
	case ESightMagnification::Two:
		TargetMagnification = 2.f;
		break;
	case ESightMagnification::TwoHalf:
		TargetMagnification = 2.5f;
		break;
	case ESightMagnification::Three:
		TargetMagnification = 3.f;
		break;
	case ESightMagnification::Four:
		TargetMagnification = 4.f;
		break;
	case ESightMagnification::Five:
		TargetMagnification = 5.f;
		break;
	default:
		break;
	}

	return InFOV / FMath::Pow(TargetMagnification, 0.9f);
}

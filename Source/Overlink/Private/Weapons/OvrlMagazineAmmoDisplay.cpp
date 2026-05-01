// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/OvrlMagazineAmmoDisplay.h"

void UOvrlMagazineAmmoDisplay::UpdateMagazineAmmo(int32 AmmoCount)
{
	if (UMaterialInstanceDynamic* Material = GetDisplayDynamicMat())
	{
		Material->SetScalarParameterValue(TEXT("Value"), AmmoCount);
	}
}

UMaterialInstanceDynamic* UOvrlMagazineAmmoDisplay::GetDisplayDynamicMat()
{
	if (!DisplayDynamicMat && GetStaticMesh())
	{
		DisplayDynamicMat = CreateDynamicMaterialInstance(0, GetMaterial(0));
	}

	return DisplayDynamicMat;
}

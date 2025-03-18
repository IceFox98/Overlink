// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Weapons/OvrlRangedWeaponReticleWidget.h"
#include "UI/Weapons/OvrlCircularReticleWidget.h"
#include "Weapons/OvrlRangedWeaponInstance.h"

void UOvrlRangedWeaponReticleWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	check(WeaponInstance);

	const float WeaponSpreadAngle = WeaponInstance->GetSpreadAngle();

	CrosshairReticle->SetRadius(WeaponSpreadAngle);
}

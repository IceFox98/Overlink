// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Weapons/OvrlRangedWeaponReticleWidget.h"
#include "UI/Weapons/OvrlCircularReticleWidget.h"
#include "Weapons/OvrlRangedWeaponInstance.h"

void UOvrlRangedWeaponReticleWidget::InitializeFromWeapon(AOvrlRangedWeaponInstance* Weapon)
{
	Super::InitializeFromWeapon(Weapon);

	Weapon->OnHitSomething.BindUObject(this, &UOvrlRangedWeaponReticleWidget::OnWeaponHitSomething);
	Weapon->OnDestroyed.AddUniqueDynamic(this, &UOvrlRangedWeaponReticleWidget::OnWeaponDestroyed);
}

void UOvrlRangedWeaponReticleWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (ensure(WeaponInstance))
	{
		const float WeaponSpreadAngleRad = FMath::DegreesToRadians(WeaponInstance->GetSpreadAngle());
		const float FixedWeaponDistance = 1000.f;

		// We consider a right triangle created by tracing an imaginary line that goes from the center of the player camera
		// and ends after "FixedWeaponDistance" units, this will be the side adjacent to the 90° angle.
		// We have to calculate the shortest side of the triangle, that will represent the radius of the crosshair.
		// We can find it using the Tan function on the spread angle
		const float SpreadRadius = FixedWeaponDistance * FMath::Tan(WeaponSpreadAngleRad);

		CrosshairReticle->SetRadius(SpreadRadius);
		CrosshairReticle->SetVisibility(WeaponInstance->IsADS() ? ESlateVisibility::Hidden : ESlateVisibility::HitTestInvisible);
	}
}

void UOvrlRangedWeaponReticleWidget::OnWeaponHitSomething(const FHitResult& HitData)
{
	K2_ShowHitMarker();
}

void UOvrlRangedWeaponReticleWidget::OnWeaponDestroyed(AActor* DestroyedActor)
{
	RemoveFromParent();
}

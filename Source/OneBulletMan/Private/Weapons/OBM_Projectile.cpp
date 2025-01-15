// Copyright Epic Games, Inc. All Rights Reserved.

#include "Weapons/OBM_Projectile.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "AbilitySystemGlobals.h"

#include "Kismet/GameplayStatics.h"

AOBM_Projectile::AOBM_Projectile()
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	SetRootComponent(CollisionComp);

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 11000.f;
	ProjectileMovement->MaxSpeed = 11000.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->bSweepCollision = false;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void AOBM_Projectile::BeginPlay()
{
	Super::BeginPlay();

	CollisionComp->OnComponentHit.AddDynamic(this, &AOBM_Projectile::OnProjectileHit);
}

void AOBM_Projectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(HitPawn);
		UAbilitySystemComponent* InstigatorASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetInstigator());

		if (InstigatorASC)
		{
			UGameplayEffect* GameplayEffect = GE_Damage->GetDefaultObject<UGameplayEffect>();
			InstigatorASC->ApplyGameplayEffectToTarget(GameplayEffect, TargetASC, 1.f, InstigatorASC->MakeEffectContext());
		}

	}

	UGameplayStatics::ApplyDamage(OtherActor, 20.f, nullptr, this, UDamageType::StaticClass());
	
	Destroy();
}

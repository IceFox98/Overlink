// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/OvrlTurret.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AOvrlTurret::AOvrlTurret()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetCapsuleHalfHeight(66.f);
	Capsule->SetCapsuleRadius(25.f);
	SetRootComponent(Capsule);

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BaseMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BaseMesh->SetupAttachment(Capsule);

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretMesh"));
	TurretMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TurretMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TurretMesh->SetupAttachment(BaseMesh);

	CannonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CannonMesh"));
	CannonMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CannonMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CannonMesh->SetupAttachment(TurretMesh);

	Muzzle = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
	Muzzle->SetupAttachment(CannonMesh);

	Laser = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Laser"));
	Laser->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Laser->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Laser->SetGenerateOverlapEvents(false);
	Laser->SetupAttachment(Muzzle);

	RPS = 3.f; // Bullets per second
	FireInitialDelay = 1.f;
	SnapRotationSpeed = 500.f;
	InterpSpeed = 10.f;
	LaserEndPointOffset = FVector(0.f, 0.f, 5.f);

	FireRate = 1.f / RPS;
}

void AOvrlTurret::BeginPlay()
{
	Super::BeginPlay();

}

void AOvrlTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!TargetPawn) return;

	FRotator LookAtTargetRotation = UKismetMathLibrary::FindLookAtRotation(CannonMesh->GetComponentLocation(), TargetPawn->GetActorLocation() + LaserEndPointOffset);
	FRotator TurretRotation = LookAtTargetRotation;

	if (InterpSpeed > 0.f)
	{
		InterpSpeed += SnapRotationSpeed * DeltaTime;
		
		if (InterpSpeed >= 100.f)
		{
			InterpSpeed = 0.f;
			Laser->SetHiddenInGame(false, true);
		}

		TurretRotation = UKismetMathLibrary::RInterpTo(TurretMesh->GetRelativeRotation(), LookAtTargetRotation, DeltaTime, InterpSpeed);
	}

	TurretMesh->SetRelativeRotation(FRotator(0.f, TurretRotation.Yaw, 0.f));
	CannonMesh->SetRelativeRotation(FRotator(LookAtTargetRotation.Pitch, 0.f, 0.f));

	const float DistanceToTarget = FVector::Distance(Muzzle->GetComponentLocation(), TargetPawn->GetActorLocation() + LaserEndPointOffset);

	FVector LaserScale = Laser->GetRelativeScale3D();
	LaserScale.Z = DistanceToTarget / 100.f;

	Laser->SetRelativeScale3D(LaserScale);
}

void AOvrlTurret::SetTarget(APawn* InTargetPawn)
{
	TargetPawn = InTargetPawn;

	if (TargetPawn)
	{
		InterpSpeed = 10.f;
		GetWorldTimerManager().SetTimer(TimerHandle_Fire, this, &AOvrlTurret::Fire, FireRate, true, FireInitialDelay);
	}
	else
	{
		Laser->SetHiddenInGame(true, true);
		GetWorldTimerManager().ClearTimer(TimerHandle_Fire);
	}
}

void AOvrlTurret::Fire()
{
	if (TargetPawn->IsValidLowLevelFast())
	{
		const FVector MuzzleLocation = Muzzle->GetComponentLocation();
		const FRotator ProjectileRotation = UKismetMathLibrary::FindLookAtRotation(MuzzleLocation, TargetPawn->GetActorLocation());

		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = this;

		GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, ProjectileRotation, SpawnParams);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_Fire);
	}
}

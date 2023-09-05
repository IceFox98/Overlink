// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Components/HealthComponent.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	MaxHealth = 100.f;
	bIsDead = false;

}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* MyOwner = GetOwner();

	if (MyOwner)
	{
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeAnyDamage);
	}
}

void UHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.f || bIsDead) return;

	// Update health clamped
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);

	//UE_LOG(LogTemp, Warning, TEXT("Health Changed: %s"), *FString::SanitizeFloat(Health));

	bIsDead = Health <= 0;

	if (bIsDead)
	{
		//ASGameMode* GM = Cast<ASGameMode>(GetWorld()->GetAuthGameMode());
		//if (GM)
		//{
		//	GM->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
		//}
	}
}

void UHealthComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0 || Health <= 0) return;

	Health = FMath::Clamp(Health + HealAmount, 0.f, MaxHealth);

	//UE_LOG(LogTemp, Warning, TEXT("Health Changed: %s"), *FString::SanitizeFloat(Health));

	OnHealthChanged.Broadcast(this, Health, -HealAmount, nullptr, nullptr, nullptr);
}

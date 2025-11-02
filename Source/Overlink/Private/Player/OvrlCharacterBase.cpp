// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/OvrlCharacterBase.h"
#include "Player/Components/OvrlHealthComponent.h"
#include "AbilitySystem/OvrlAbilitySet.h"
#include "AbilitySystem/OvrlAbilitySystemComponent.h"

#include "OvrlUtils.h"

// Sets default values
AOvrlCharacterBase::AOvrlCharacterBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UOvrlHealthComponent>(TEXT("HealthComponent"));
	AbilitySystemComponent = CreateDefaultSubobject<UOvrlAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

	GripPointName = FName("GripPoint");
}

// Called when the game starts or when spawned
void AOvrlCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	HealthComponent->InitializeWithASC(AbilitySystemComponent);
	HealthComponent->OnOutOfHealth.AddDynamic(this, &AOvrlCharacterBase::HandleDeath);

	for (const UOvrlAbilitySet* AbilitySet : AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr, this);
		}
	}
}

// Called every frame
void AOvrlCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AOvrlCharacterBase::HandleDeath(AActor* InInstigator)
{
	OVRL_LOG_INFO(LogTemp, false, "%s is out of health, destroying. Killer: %s", *GetName(), *GetNameSafe(InInstigator));

	Destroy();
}

void AOvrlCharacterBase::EquipObject(AActor* ObjectToEquip, UStaticMesh* MeshToDisplay)
{
	// MeshToDisplay is not used here, look at PlayerCharacter

	if (ensure(ObjectToEquip))
	{
		ObjectToEquip->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, GripPointName);
	}
}

void AOvrlCharacterBase::PlayAnimMontage(UAnimMontage* MontageToPlay, float StartTime)
{
	GetMesh()->GetAnimInstance()->Montage_Play(MontageToPlay, 1.f, EMontagePlayReturnType::Duration, StartTime);
}

UAbilitySystemComponent* AOvrlCharacterBase::GetAbilitySystemComponent() const
{
	return GetOvrlAbilitySystemComponent();
}

// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/OvrlCharacterBase.h"
#include "Player/Components/OvrlHealthComponent.h"
#include "Animations/OvrlLinkedAnimInstance.h"
#include "AbilitySystem/OvrlAbilitySet.h"
#include "AbilitySystem/OvrlAbilitySystemComponent.h"
#include "Player/Components/OvrlCharacterMovementComponent.h"
#include "OvrlLogUtils.h"

// Sets default values
AOvrlCharacterBase::AOvrlCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UOvrlCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UOvrlHealthComponent>(TEXT("HealthComponent"));
	AbilitySystemComponent = CreateDefaultSubobject<UOvrlAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	
	LandedResetTime = .3f;
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
	
	GetCharacterMovement()->OnPlayMontageRequested.AddDynamic(this, &AOvrlCharacterBase::OvrlPlayAnimMontage);
}

void AOvrlCharacterBase::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	
	bJustLanded = true;
	
	GetWorldTimerManager().ClearTimer(TimerHandle_LandReset);
	GetWorldTimerManager().SetTimer(TimerHandle_LandReset, [this]() {
		bJustLanded = false;
	}, LandedResetTime, false);
}

void AOvrlCharacterBase::ApplyAnimLayerClass(const TSubclassOf<UOvrlLinkedAnimInstance>& LayerClass)
{
	if (GetMesh())
	{
		GetMesh()->LinkAnimClassLayers(LayerClass);
	}
	
	bHasDefaultAnimLayerClass = false;
}

void AOvrlCharacterBase::RestoreAnimLayerClass()
{
	if (GetMesh())
	{
		GetMesh()->LinkAnimClassLayers(DefaultAnimLayerClass);
	}
	
	bHasDefaultAnimLayerClass = true;
}

void AOvrlCharacterBase::HandleDeath(AActor* InInstigator)
{
	OVRL_LOG_INFO(LogOverlink, false, "%s is out of health, destroying. Killer: %s", *GetName(), *GetNameSafe(InInstigator));

	Destroy();
}

void AOvrlCharacterBase::EquipObject(AActor* ObjectToEquip, FName AttachSocketName, UStaticMesh* MeshToDisplay)
{
	// MeshToDisplay is not used here, look at PlayerCharacter

	if (ensure(ObjectToEquip))
	{
		ObjectToEquip->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, AttachSocketName);
	}
}

void AOvrlCharacterBase::UnequipObject()
{
}

// Overridden in Player Character, to play full body animation
void AOvrlCharacterBase::OvrlPlayAnimMontage(UAnimMontage* MontageToPlay, float StartTime)
{
	GetMesh()->GetAnimInstance()->Montage_Play(MontageToPlay, 1.f, EMontagePlayReturnType::Duration, StartTime);
}

// Overridden in Player Character, to play full body animation
void AOvrlCharacterBase::OvrlStopAnimMontage(UAnimMontage* MontageToStop)
{
	GetMesh()->GetAnimInstance()->Montage_Stop(MontageToStop->BlendOut.GetBlendTime(), MontageToStop);
}

UAbilitySystemComponent* AOvrlCharacterBase::GetAbilitySystemComponent() const
{
	return GetOvrlAbilitySystemComponent();
}

UOvrlCharacterMovementComponent* AOvrlCharacterBase::GetCharacterMovement() const
{
	return Cast<UOvrlCharacterMovementComponent>(GetMovementComponent());
}

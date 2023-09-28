// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Components/InteractionComponent.h"
#include "Player/CharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f;

	Range = 600.f;
}

// Called when the game starts
void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	CharacterRef = Cast<ACharacterBase>(GetOwner());
	LastActorHit = nullptr;
}

// Called every frame
void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//if (!CharacterRef) return;

	//FInteractionData Data = FindInteractiveObject();

	//if (LastActorHit && (LastActorHit != Data.HitActor || !Data.IsInteractive))
	//{
	//	Cast<IInteractable>(LastActorHit)->Execute_OnEndFocus(LastActorHit, CharacterRef->GetController());
	//	// Stop continuous call of OnEndFocus
	//	LastActorHit = nullptr;
	//}

	//if (Data.HitActor)
	//{
	//	if (Data.IsInteractive)
	//	{
	//		Cast<IInteractable>(Data.HitActor)->Execute_OnBeginFocus(Data.HitActor, CharacterRef->GetController(), Data.HitComponent);
	//		LastActorHit = Data.HitActor;
	//	}
	//}
	//else
	//{
	//	LastActorHit = nullptr;
	//}
}

FInteractionData UInteractionComponent::FindInteractiveObject()
{
	//// Get Player's camera Direction
	//APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);

	//FVector StartLocation = CameraManager->GetCameraLocation();
	//FVector LineTraceEnd = StartLocation + (CameraManager->GetActorForwardVector() * Range);

	//// Trace a ray that "has" the Visibility channel
	//FHitResult Hit;
	//GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, LineTraceEnd, ECollisionChannel::ECC_Visibility);

	FInteractionData InteractionData;
	InteractionData.IsInteractive = false;

	//if (AActor* HitActor = Hit.GetActor())
	//{
	//	InteractionData.HitActor = HitActor;
	//	InteractionData.HitComponent = Hit.GetComponent();

	//	if (HitActor->Implements<UInteractable>())
	//	{
	//		InteractionData.IsInteractive = Cast<IInteractable>(HitActor)->Execute_IsComponentInteractive(HitActor, InteractionData.HitComponent);
	//	}
	//}

	return InteractionData;
}

void UInteractionComponent::ChangeRange(float NewRange)
{
	Range = NewRange;
}

#include "Player/Components/OvrlInteractionComponent.h"

// Internal
#include "Core/Interfaces/OvrlInteractable.h"

// Engine
#include "OvrlUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/KismetSystemLibrary.h"

UOvrlInteractionComponent::UOvrlInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	InteractionDistance = 600.f;
	
	TraceTypes.Add(EObjectTypeQuery::ObjectTypeQuery1); // World Static
	TraceTypes.Add(EObjectTypeQuery::ObjectTypeQuery2); // World Dynamic
}

void UOvrlInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	OwningPawn = Cast<APawn>(GetOwner());
}

void UOvrlInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FInteractableObjectData Data = FindInteractableObject();

	// If it's not null, then is an Interactable object for sure
	const bool bIsInteractable = Data.OriginalObject != nullptr;
	
	if (Data.OriginalObject == CurrentPointedObjData.OriginalObject)
	{
		if (bIsInteractable && IOvrlInteractable::Execute_ShouldReceiveHoveringUpdate(CurrentPointedObjData.OriginalObject))
		{
			IOvrlInteractable::Execute_HoveringUpdate(CurrentPointedObjData.OriginalObject, FHitResult());
		}
	}
	else // If the pointed object changes
	{
		// If bIsInteractable is true, then ObjectToGrab is Valid for sure
		// The check for the Interface should have already been done
		if (bIsInteractable && IOvrlInteractable::Execute_CanInteract(Data.OriginalObject))
		{
			if (CurrentPointedObjData.OriginalObject)
			{
				IOvrlInteractable::Execute_EndHover(CurrentPointedObjData.OriginalObject);
				OnObjectEndHovered.Broadcast(CurrentPointedObjData.OriginalObject);
			}

			// Update PointedObjData reference
			CurrentPointedObjData = Data;
			IOvrlInteractable::Execute_BeginHover(CurrentPointedObjData.OriginalObject, OwningPawn);
			OnObjectBeginHovered.Broadcast(CurrentPointedObjData.OriginalObject);
		}
		else
		{
			if (CurrentPointedObjData.OriginalObject)
			{
				IOvrlInteractable::Execute_EndHover(CurrentPointedObjData.OriginalObject);
				OnObjectEndHovered.Broadcast(CurrentPointedObjData.OriginalObject);
				CurrentPointedObjData = FInteractableObjectData(); // Reset struct data
			}
		}
	}
}

FInteractableObjectData UOvrlInteractionComponent::FindInteractableObject_Implementation() const
{
	FVector TraceStart, TraceEnd;
	GetTraceStartEnd(TraceStart, TraceEnd);

	EDrawDebugTrace::Type DebugType = EDrawDebugTrace::None;

#if ENABLE_DRAW_DEBUG
	const bool bDebugEnabled = UOvrlUtils::ShouldDisplayDebugForActor(OwningPawn, "Ovrl.Interactions");

	if (bDebugEnabled)
		DebugType = EDrawDebugTrace::ForOneFrame;
#endif

	FHitResult OutHit;
	UKismetSystemLibrary::LineTraceSingleForObjects(this, TraceStart, TraceEnd, TraceTypes, bTraceComplex, { OwningPawn }, DebugType, OutHit, true);

	if (OutHit.bBlockingHit)
	{
		// Search for components first
		UPrimitiveComponent* ComponentHit = OutHit.GetComponent();
		if (ComponentHit && ComponentHit->Implements<UOvrlInteractable>())
		{
			// Returns the first Component that implements the Interactable interface
			return CreateInteractableData(ComponentHit, ComponentHit->GetOwner());
		}

		// Fallback to Actors
		AActor* ActorHit = OutHit.GetActor();
		if (ActorHit && ActorHit->Implements<UOvrlInteractable>())
		{
			// Returns the first Actor that implements the Interactable interface
			return CreateInteractableData(ActorHit, ActorHit);
		}
	}

	// No interactable object found
	return FInteractableObjectData();
}

void UOvrlInteractionComponent::GetTraceStartEnd_Implementation(FVector& OutTraceStart, FVector& OutTraceEnd) const
{
	// Get Player's camera Direction
	const APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);

	OutTraceStart = CameraManager->GetCameraLocation();
	OutTraceEnd = OutTraceStart + (CameraManager->GetActorForwardVector() * InteractionDistance);
}

bool UOvrlInteractionComponent::IsInteractableObject(UObject* ObjectToCheck) const
{
	return ObjectToCheck && ObjectToCheck->Implements<UOvrlInteractable>();
}

FInteractableObjectData UOvrlInteractionComponent::CreateInteractableData(UObject* OriginalObject, AActor* OwningActor) const
{
	return FInteractableObjectData(OriginalObject, OriginalObject, OwningActor);
}

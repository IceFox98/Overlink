// Fill out your copyright notice in the Description page of Project Settings.


#include "OvrlUtils.h"
#include "GameFramework/HUD.h"
#include "Core/OvrlPlayerCameraManager.h"

FRotator UOvrlUtils::GetGravityRelativeRotation(FRotator Rotation, FVector GravityDirection)
{
	if (!GravityDirection.Equals(FVector::DownVector))
	{
		// Get the rotation needed to rotate the GravityDirection to be aligned with Down Vector
		FQuat GravityRotation = FQuat::FindBetweenNormals(GravityDirection, FVector::DownVector);

		// Transform the original Rotation to be adapted to the gravity direction
		return (GravityRotation * Rotation.Quaternion()).Rotator();
	}

	return Rotation;
}

FRotator UOvrlUtils::GetGravityWorldRotation(FRotator Rotation, FVector GravityDirection)
{
	if (!GravityDirection.Equals(FVector::DownVector))
	{
		FQuat GravityRotation = FQuat::FindBetweenNormals(FVector::DownVector, GravityDirection);
		return (GravityRotation * Rotation.Quaternion()).Rotator();
	}

	return Rotation;
}

FTransform UOvrlUtils::ExtractRootTransformFromMontage(const UAnimMontage* Montage, float Time)
{
	// Based on UMotionWarpingUtilities::ExtractRootTransformFromAnimation().

	if (!IsValid(Montage) || Montage->SlotAnimTracks.Num() <= 0)
	{
		return FTransform::Identity;
	}

	const auto* Segment = Montage->SlotAnimTracks[0].AnimTrack.GetSegmentAtTime(Time);
	if (!Segment)
	{
		return FTransform::Identity;
	}

	const auto* Sequence = Cast<UAnimSequence>(Segment->GetAnimReference());
	if (!Sequence)
	{
		return FTransform::Identity;
	}

	return Sequence->ExtractRootTrackTransform(Segment->ConvertTrackPosToAnimPos(Time), nullptr);
}

bool UOvrlUtils::ShouldDisplayDebugForActor(const AActor* Actor, const FName& DisplayName)
{
	const UWorld* World = IsValid(Actor) ? Actor->GetWorld() : nullptr;
	const APlayerController* PC = IsValid(World) ? World->GetFirstPlayerController() : nullptr;
	AHUD* Hud = IsValid(PC) ? PC->GetHUD() : nullptr;

	return IsValid(Hud) && Hud->ShouldDisplayDebug(DisplayName) && Hud->GetCurrentDebugTargetActor() == Actor;
}

void UOvrlUtils::TriggerCameraEvent(UObject* WorldObjectContext, ECameraFeedbackEvent CameraEvent)
{
	if (AOvrlPlayerCameraManager* PCM = AOvrlPlayerCameraManager::Get(WorldObjectContext))
	{
		PCM->HandleCameraEvent(CameraEvent, 1.f);
	}
}

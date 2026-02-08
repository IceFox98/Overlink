// Fill out your copyright notice in the Description page of Project Settings.

#include "OvrlUtils.h"

#include "Animations/OvrlAnimNode_OffsetRootBone.h"
#include "Animation/AnimNodeReference.h"
#include "GameFramework/HUD.h"
#include "Core/OvrlPlayerCameraManager.h"

FRotator UOvrlUtils::GetGravityRelativeRotation(FRotator Rotation, FVector GravityDirection)
{
	if (!GravityDirection.Equals(FVector::DownVector))
	{
		// Get the rotation needed to rotate the GravityDirection to be aligned with Down Vector
		const FQuat GravityRotation = FQuat::FindBetweenNormals(GravityDirection, FVector::DownVector);

		// Transform the original Rotation to be adapted to the gravity direction
		return (GravityRotation * Rotation.Quaternion()).Rotator();
	}

	return Rotation;
}

FQuat UOvrlUtils::GetGravityRelativeRotation_Quat(FQuat Rotation, FVector GravityDirection)
{
	if (!GravityDirection.Equals(FVector::DownVector))
	{
		// Get the rotation needed to rotate the GravityDirection to be aligned with Down Vector
		const FQuat GravityRotation = FQuat::FindBetweenNormals(GravityDirection, FVector::DownVector);

		// Transform the original Rotation to be adapted to the gravity direction
		return GravityRotation * Rotation;
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

FTransform UOvrlUtils::GetOffsetRootTransform(const FAnimNodeReference& Node)
{
	FTransform Transform(FTransform::Identity);

	if (FOvrlAnimNode_OffsetRootBone* OffsetRoot = Node.GetAnimNodePtr<FOvrlAnimNode_OffsetRootBone>())
	{
		OffsetRoot->GetOffsetRootTransform(Transform);
	}

	return Transform;
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

void UOvrlUtils::CriticalSpringDamperQuat(FQuat& InOutRotation, FVector& InOutAngularVelocityRadians, const FQuat& TargetRotation, float SmoothingTime, float DeltaTime)
{
	float Y = SmoothingTimeToDamping(SmoothingTime) / 2.0f;

	FQuat Diff = InOutRotation * TargetRotation.Inverse();
	Diff.EnforceShortestArcWith(FQuat::Identity);
	FVector J0 = Diff.ToRotationVector();
	FVector J1 = InOutAngularVelocityRadians + J0 * Y;

	float EyDt = FMath::InvExpApprox(Y * DeltaTime);

	InOutRotation = FQuat::MakeFromRotationVector(EyDt * (J0 + J1 * DeltaTime)) * TargetRotation;
	InOutAngularVelocityRadians = EyDt * (InOutAngularVelocityRadians - J1 * Y * DeltaTime);
}

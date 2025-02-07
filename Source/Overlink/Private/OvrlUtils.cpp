// Fill out your copyright notice in the Description page of Project Settings.


#include "OvrlUtils.h"

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

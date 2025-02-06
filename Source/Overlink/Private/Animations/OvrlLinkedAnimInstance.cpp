// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/OBM_LinkedAnimInstance.h"

#include "Player/OBM_PlayerAnimInstance.h"
#include "Player/OBM_PlayerCharacter.h"

UOBM_LinkedAnimInstance::UOBM_LinkedAnimInstance()
{
	bUseMainInstanceMontageEvaluationData = true;
}

void UOBM_LinkedAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Parent = Cast<UOBM_PlayerAnimInstance>(GetSkelMeshComponent()->GetAnimInstance());
	Character = Cast<AOBM_PlayerCharacter>(GetOwningActor());

#if WITH_EDITOR
	const auto* World{ GetWorld() };

	if (IsValid(World) && !World->IsGameWorld())
	{
		// Use default objects for editor preview.

		if (!Parent.IsValid())
		{
			Parent = GetMutableDefault<UOBM_PlayerAnimInstance>();
		}

		if (!IsValid(Character))
		{
			Character = GetMutableDefault<AOBM_PlayerCharacter>();
		}
	}
#endif
}

void UOBM_LinkedAnimInstance::NativeBeginPlay()
{
	ensureAlwaysMsgf(Parent.IsValid(), TEXT("%s should only be used as a linked animation instance within the OBM_PlayerAnimInstance animation blueprint!"));

	Super::NativeBeginPlay();
}

UOBM_PlayerAnimInstance* UOBM_LinkedAnimInstance::GetParent() const
{
	return Parent.Get();
}

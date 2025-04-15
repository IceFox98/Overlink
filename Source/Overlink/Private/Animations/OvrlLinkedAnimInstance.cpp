// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/OvrlLinkedAnimInstance.h"

#include "Player/OvrlPlayerAnimInstance.h"
#include "Player/OvrlPlayerCharacter.h"

UOvrlLinkedAnimInstance::UOvrlLinkedAnimInstance()
{
	bUseMainInstanceMontageEvaluationData = true;
}

void UOvrlLinkedAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Parent = Cast<UOvrlPlayerAnimInstance>(GetSkelMeshComponent()->GetAnimInstance());
	PlayerCharacter = Cast<AOvrlPlayerCharacter>(GetOwningActor());

#if WITH_EDITOR
	const auto* World{ GetWorld() };

	if (IsValid(World) && !World->IsGameWorld())
	{
		// Use default objects for editor preview.

		if (!Parent.IsValid())
		{
			Parent = GetMutableDefault<UOvrlPlayerAnimInstance>();
		}

		if (!IsValid(PlayerCharacter))
		{
			PlayerCharacter = GetMutableDefault<AOvrlPlayerCharacter>();
		}
	}
#endif
}

void UOvrlLinkedAnimInstance::NativeBeginPlay()
{
	ensureAlwaysMsgf(Parent.IsValid(), TEXT("%s should only be used as a linked animation instance within the OvrlPlayerAnimInstance animation blueprint!"));

	Super::NativeBeginPlay();
}

UOvrlPlayerAnimInstance* UOvrlLinkedAnimInstance::GetParent() const
{
	return Parent.Get();
}

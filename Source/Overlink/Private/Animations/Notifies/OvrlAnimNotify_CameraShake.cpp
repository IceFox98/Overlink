#include "Animations/Notifies/OvrlAnimNotify_CameraShake.h"

#include "Camera/CameraShakeBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OvrlAnimNotify_CameraShake)

UOvrlAnimNotify_CameraShake::UOvrlAnimNotify_CameraShake()
{
#if WITH_EDITORONLY_DATA
	bShouldFireInEditor = false;
#endif
}

FString UOvrlAnimNotify_CameraShake::GetNotifyName_Implementation() const
{
	TStringBuilder<256> NotifyNameBuilder{InPlace, TEXTVIEW("Ovrl Camera Shake: ")};

	if (IsValid(CameraShakeClass))
	{
		NotifyNameBuilder << CameraShakeClass->GetFName();
	}

	return FString{NotifyNameBuilder};
}

void UOvrlAnimNotify_CameraShake::Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Sequence, const FAnimNotifyEventReference& NotifyEventReference)
{
	Super::Notify(Mesh, Sequence, NotifyEventReference);

	if (const APawn* Pawn = Cast<APawn>(Mesh->GetOwner()))
	{
		if (const APlayerController* PC = Cast<APlayerController>(Pawn->GetController()))
		{
			if (APlayerCameraManager* CameraManager = PC->PlayerCameraManager)
			{
				CameraManager->StartCameraShake(CameraShakeClass, CameraShakeScale);
			}
		}
	}
}

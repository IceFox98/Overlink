#include "Animations/Notifies/OvrlAnimNotify_PlaySound.h"

#include "Player/OvrlPlayerCharacter.h"

#include "AnimPreviewInstance.h"
#include "Components/SkeletalMeshComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(OvrlAnimNotify_PlaySound)

void UOvrlAnimNotify_PlaySound::Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Sequence, const FAnimNotifyEventReference& NotifyEventReference)
{
	bool bCanPlayNotify = false;

	// Since player plays the same animation on multiple skeletal mesh, we play the sound only on one of them.
	if (AOvrlPlayerCharacter* PlayerCharacter = Cast<AOvrlPlayerCharacter>(Mesh->GetOwner()))
	{
		bCanPlayNotify = PlayerCharacter->GetFullBodyMesh() == Mesh;
	}

	// When the notify is played from anim preview, the Mesh is of type UAnimPreviewInstance.
	// We use this trick to make it play from preview too.
#if WITH_EDITOR
	if (!bCanPlayNotify)
	{
		bCanPlayNotify = Cast<UAnimPreviewInstance>(Mesh->GetAnimInstance()) != nullptr;
	}
#endif

	if (bCanPlayNotify)
	{
		Super::Notify(Mesh, Sequence, NotifyEventReference);
	}
}

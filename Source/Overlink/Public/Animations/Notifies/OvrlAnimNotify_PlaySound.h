#pragma once

#include "Animation/AnimNotifies/AnimNotify_PlaySound.h"
#include "OvrlAnimNotify_PlaySound.generated.h"

UCLASS(DisplayName = "Ovrl Play Sound")
class OVERLINK_API UOvrlAnimNotify_PlaySound : public UAnimNotify_PlaySound
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Sequence, const FAnimNotifyEventReference& NotifyEventReference) override;
};

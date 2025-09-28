#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "Templates/SubclassOf.h"
#include "OvrlAnimNotify_CameraShake.generated.h"

class UCameraShakeBase;

UCLASS(DisplayName = "Ovrl Camera Shake Animation Notify")
class OVERLINK_API UOvrlAnimNotify_CameraShake : public UAnimNotify
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings")
	TSubclassOf<UCameraShakeBase> CameraShakeClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", Meta = (ClampMin = 0, ForceUnits = "x"))
	float CameraShakeScale = 1.0f;

public:
	UOvrlAnimNotify_CameraShake();

	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* Mesh, UAnimSequenceBase* Sequence,
	                    const FAnimNotifyEventReference& NotifyEventReference) override;
};

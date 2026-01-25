// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "OvrlActivatableWidget.generated.h"

struct FUIInputConfig;

UENUM(BlueprintType)
enum class EInvrWidgetInputMode : uint8
{
	Default,
	GameAndMenu,
	Game,
	Menu
};

/**
 *
 */
UCLASS()
class OVERLINK_API UOvrlActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()


public:
	UOvrlActivatableWidget(const FObjectInitializer& ObjectInitializer);

public:

	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

protected:
	/** The desired input mode to use while this UI is activated, for example do you want key presses to still reach the game/player controller? */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	EInvrWidgetInputMode InputConfig = EInvrWidgetInputMode::Default;

	/** The desired mouse behavior when the game gets input. */
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;
};


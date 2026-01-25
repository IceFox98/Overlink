// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OvrlActivatableWidget.h"

UOvrlActivatableWidget::UOvrlActivatableWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

TOptional<FUIInputConfig> UOvrlActivatableWidget::GetDesiredInputConfig() const
{
	switch (InputConfig)
	{
	case EInvrWidgetInputMode::GameAndMenu:
		return FUIInputConfig(ECommonInputMode::All, GameMouseCaptureMode);
	case EInvrWidgetInputMode::Game:
		return FUIInputConfig(ECommonInputMode::Game, GameMouseCaptureMode);
	case EInvrWidgetInputMode::Menu:
		return FUIInputConfig(ECommonInputMode::Menu, EMouseCaptureMode::NoCapture);
	case EInvrWidgetInputMode::Default:
	default:
		return TOptional<FUIInputConfig>();
	}
}

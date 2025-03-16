// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OvrlGameUIManagerSubsystem.h"
#include "UI/OvrlBaseUILayers.h"

#include "Blueprint/UserWidget.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "CommonActivatableWidget.h"

#include "OvrlUtils.h"
#include "Overlink.h"

void UOvrlGameUIManagerSubsystem::CreateBaseWidget(TSoftClassPtr<UOvrlBaseUILayers> BaseLayerWidgetClass)
{
	if (!BaseLayerWidgetClass.IsValid())
	{
		OVRL_LOG_ERR(LogOverlink, true, "BaseLayerWidgetClass is NULL!");
		return;
	}

	BaseLayersWidget = CreateWidget<UOvrlBaseUILayers>(GetGameInstance(), BaseLayerWidgetClass.LoadSynchronous());

	if (!BaseLayersWidget)
	{
		OVRL_LOG_ERR(LogOverlink, true, "Failed to create base widget.");
		return;
	}

	BaseLayersWidget->AddToViewport();
}

void UOvrlGameUIManagerSubsystem::AddWidgetToLayer(FGameplayTag LayerName, TSoftClassPtr<UCommonActivatableWidget> WidgetClass)
{
	if (!IsValid(BaseLayersWidget))
	{
		OVRL_LOG_ERR(LogOverlink, true, "BaseLayersWidget is NULL!");
		return;
	}

	if (!WidgetClass.IsValid())
	{
		OVRL_LOG_ERR(LogOverlink, true, "WidgetClass is NULL!");
		return;
	}

	if (UCommonActivatableWidgetContainerBase* LayerWidget = BaseLayersWidget->GetLayerWidget(LayerName))
	{
		LayerWidget->AddWidget(WidgetClass.LoadSynchronous());
	}
	else
	{
		OVRL_LOG_ERR(LogOverlink, true, "Layer not found.");
	}
}

void UOvrlGameUIManagerSubsystem::NotifyPlayerDestroyed(ULocalPlayer* LocalPlayer)
{
	// Remove base widget on player detroyed
	BaseLayersWidget->RemoveFromParent();
}

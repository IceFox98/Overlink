// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/OvrlBaseUILayers.h"

void UOvrlBaseUILayers::RegisterLayer(FGameplayTag LayerName, UCommonActivatableWidgetContainerBase* LayerWidget)
{
	check(LayerWidget);

	Layers.Add(LayerName, LayerWidget);
}

UCommonActivatableWidgetContainerBase* UOvrlBaseUILayers::GetLayerWidget(const FGameplayTag& LayerName)
{
	return Layers.FindRef(LayerName);
}

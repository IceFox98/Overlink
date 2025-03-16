// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"

#include "OvrlBaseUILayers.generated.h"

class UCommonActivatableWidgetContainerBase;

/**
 *
 */
UCLASS()
class OVERLINK_API UOvrlBaseUILayers : public UCommonUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Ovrl Base UI Layers")
		void RegisterLayer(UPARAM(meta = (Categories = "Ovrl.UI.Layer")) FGameplayTag LayerName, UCommonActivatableWidgetContainerBase* LayerWidget);

	// Get the layer widget for the given layer tag.
	UCommonActivatableWidgetContainerBase* GetLayerWidget(const FGameplayTag& LayerName);

private:

	UPROPERTY()
		TMap<FGameplayTag, TObjectPtr<UCommonActivatableWidgetContainerBase>> Layers;
};

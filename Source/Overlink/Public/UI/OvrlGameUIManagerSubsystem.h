// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"

#include "OvrlGameUIManagerSubsystem.generated.h"

class UOvrlBaseUILayers;
class ULocalPlayer;
class UCommonActivatableWidget;

/**
 *
 */
UCLASS()
class OVERLINK_API UOvrlGameUIManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Ovrl Game UI Manager")
		void CreateBaseWidget(TSoftClassPtr<UOvrlBaseUILayers> BaseLayerWidgetClass);

	UFUNCTION(BlueprintCallable, Category = "Ovrl Game UI Manager")
		void AddWidgetToLayer(UPARAM(meta = (Categories = "Ovrl.UI.Layer")) FGameplayTag LayerName, TSoftClassPtr<UCommonActivatableWidget> WidgetClass);

	void NotifyPlayerDestroyed(ULocalPlayer* LocalPlayer);

private:

	UPROPERTY()
		TObjectPtr<UOvrlBaseUILayers> BaseLayersWidget;
};

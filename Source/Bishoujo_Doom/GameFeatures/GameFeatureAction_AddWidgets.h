// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction.h"
#include "GameFeaturesSubsystem.h"
#include "GameplayTagContainer.h"
#include "GameFeatureAction_AddWidgets.generated.h"

class UUserWidget;
class UCommonActivatableWidget;

USTRUCT(BlueprintType)
struct FGameFeatureWidgetEntry
{
	GENERATED_BODY()

	// Widget class to add
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	TSoftClassPtr<UUserWidget> WidgetClass;

	// HUD slot name where to add the widget
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	FGameplayTag SlotTag;

	// Z-order for the widget
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	int32 ZOrder = 0;

	FGameFeatureWidgetEntry()
	{
		ZOrder = 0;
	}
};

/**
 * 
 */
UCLASS(meta = (DisplayName = "Add Widgets"))
class BISHOUJO_DOOM_API UGameFeatureAction_AddWidgets : public UGameFeatureAction
{
	GENERATED_BODY()

public:
	//~UGameFeatureAction interface
	virtual void OnGameFeatureRegistering() override;
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	virtual void OnGameFeatureUnregistering() override;
	//~End of UGameFeatureAction interface

	// Array of widgets to add
	UPROPERTY(EditAnywhere, Category = "UI", meta = (TitleProperty = "SlotTag"))
	TArray<FGameFeatureWidgetEntry> Widgets;

private:
	struct FPerContextData
	{
		TArray<TWeakObjectPtr<UUserWidget>> AddedWidgets;
		FDelegateHandle ExtensionRequestHandle;
	};

	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;

	void AddWidgetsForPlayer(UPlayer* Player, FPerContextData& ActiveData);
	void OnWidgetClassesLoaded(UPlayer* Player, FPerContextData ActiveData);
	void RemoveWidgetsForPlayer(UPlayer* Player, FPerContextData& ActiveData);
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction.h"
#include "GameFeaturesSubsystem.h"
#include "GameplayTagContainer.h"
#include "UI/SubSystem/BSPlayerUISubSystem.h"
#include "GameFeatureAction_AddWidgets.generated.h"

enum EUICategory : uint8;
class UUserWidget;
class UCommonActivatableWidget;

USTRUCT(BlueprintType)
struct FGameFeatureWidgetEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	TSoftClassPtr<UUserWidget> WidgetClass;

	UPROPERTY(EditAnywhere, Category="Widget")
	TSoftClassPtr<AActor> TargetActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	TEnumAsByte<EUICategory> WidgetCategory = EUICategory::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	FGameplayTag SlotTag;

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

	UPROPERTY(EditAnywhere, Category = "UI", meta = (TitleProperty = "SlotTag"))
	TArray<FGameFeatureWidgetEntry> Widgets;

private:
	TMap<ULocalPlayer*, EUICategory> AddedWidgets;
	
	void AddWidgetsForPlayer(UPlayer* Player);
	void OnWidgetClassesLoaded(UPlayer* Player);
	void RemoveWidgetsForPlayer(UPlayer* Player);
};
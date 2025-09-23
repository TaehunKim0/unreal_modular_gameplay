// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BSLogChannels.h"
#include "Blueprint/UserWidget.h"
#include "BSPlayerUISubSystem.generated.h"

UENUM(BlueprintType)
enum EUICategory : uint8
{
	Debug,
	CharacterSelection
};
/**
 * 
 */

DECLARE_MULTICAST_DELEGATE_OneParam(FOnUICreated, EUICategory);

UCLASS()
class BISHOUJO_DOOM_API UBSPlayerUISubSystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
public:
	template<typename T>
	T* CreateWidget(TSubclassOf<T> InWidgetClass, EUICategory InCategory, APlayerController* InPlayerController)
	{
		if (!InWidgetClass || !InPlayerController)
		{
			UE_LOG(LogBS, Error, TEXT("CreateWidget subsystem returned nullptr"));
			return nullptr;
		}

		if (auto FindWidget = GetWidgetByCategory(InCategory))
		{
			return FindWidget;
		}

		T* NewWidget = ::CreateWidget<T>(InPlayerController, InWidgetClass);
		if (NewWidget)
		{
			ActiveWidgetMap.Add(InCategory, NewWidget);
			Cast<UUserWidget>(NewWidget)->AddToViewport();
			OnUICreated.Broadcast(InCategory);
		}

		return NewWidget;
	}
	
	template<typename T>
	T* GetWidget(EUICategory InCategory) const
	{
		if (ActiveWidgetMap.Contains(InCategory))
		{
			return Cast<T>(ActiveWidgetMap[InCategory]);
		}
		
		return nullptr;
	}

	UFUNCTION()
	static UBSPlayerUISubSystem* Get(const UObject* WorldContext);

	UFUNCTION()
	void ShowWidget(EUICategory InCategory) const;
	
	UFUNCTION()
	void RemoveWidget(EUICategory InCategory);

	UFUNCTION()
	void RemoveAllWidgets();

	UFUNCTION()
	void HideWidget(EUICategory InCategory);

	UFUNCTION(BlueprintCallable)
	UUserWidget* GetWidgetByCategory(EUICategory InCategory) const;

public:
	void ShowDebugMessage(const FString& InVariableName, const FString& InDesc) const;

public:
	FOnUICreated OnUICreated;

protected:
	
	// 카테고리별 위젯 맵
	UPROPERTY()
	TMap<TEnumAsByte<EUICategory>, UUserWidget*> ActiveWidgetMap;
};
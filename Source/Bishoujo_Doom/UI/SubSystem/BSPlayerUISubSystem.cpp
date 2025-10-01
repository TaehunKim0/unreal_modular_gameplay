// Fill out your copyright notice in the Description page of Project Settings.


#include "BSPlayerUISubSystem.h"

#include "BSLogChannels.h"
#include "Blueprint/UserWidget.h"
#include "UI/Debug/BSDebugWidget.h"

void UBSPlayerUISubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogBS, Log, TEXT("UBSPlayerUISubSystem::FirstInitialize"));
}

void UBSPlayerUISubSystem::Deinitialize()
{
	RemoveAllWidgets();
	
	Super::Deinitialize();
}

void UBSPlayerUISubSystem::ShowWidget(EUICategory InCategory) const
{
	if (UUserWidget* Widget = GetWidgetByCategory(InCategory))
	{
		Widget->SetVisibility(ESlateVisibility::Visible);
	}
}

void UBSPlayerUISubSystem::RemoveWidget(EUICategory InCategory)
{
	if (UUserWidget* Widget = GetWidgetByCategory(InCategory))
	{
		Widget->RemoveFromParent();
		ActiveWidgetMap.Remove(InCategory);
	}
}

void UBSPlayerUISubSystem::HideWidget(EUICategory InCategory)
{
	if (UUserWidget* Widget = GetWidgetByCategory(InCategory))
	{
		Widget->SetVisibility(ESlateVisibility::Hidden);
	}
}

UUserWidget* UBSPlayerUISubSystem::GetWidgetByCategory(EUICategory InCategory) const
{
	if (ActiveWidgetMap.IsEmpty())
	{
		return nullptr;
	}
	
	if (const auto FoundWidget = ActiveWidgetMap.Find(InCategory))
	{
		return *FoundWidget;
	}

	return nullptr;
}

void UBSPlayerUISubSystem::ShowDebugMessage(const FString& InVariableName, const FString& InDesc) const
{
	if (UUserWidget* Widget = GetWidgetByCategory(Debug))
	{
		if (UBSDebugWidget* const DebugWidget = Cast<UBSDebugWidget>(Widget))
		{
			DebugWidget->ShowDebugMessage(InVariableName, InDesc);
		}
		else
		{
			UE_LOG(LogBS, Error, TEXT("UBSPlayerUISubSystem::DebugWidget is Null"));
		}
	}
	else
	{
		UE_LOG(LogBS, Error, TEXT("UBSPlayerUISubSystem::Widget is Null"));
	}
}

void UBSPlayerUISubSystem::RemoveAllWidgets()
{
	for (auto& WidgetPair : ActiveWidgetMap)
	{
		if (WidgetPair.Value)
		{
			WidgetPair.Value->RemoveFromParent();
		}
	}
    
	ActiveWidgetMap.Empty();
}


UBSPlayerUISubSystem* UBSPlayerUISubSystem::Get(const UObject* WorldContext)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			if (ULocalPlayer* LocalPlayer = GameInstance->GetFirstGamePlayer())
			{
				return LocalPlayer->GetSubsystem<UBSPlayerUISubSystem>();
			}
		}
	}

	UE_LOG(LogBS, Log, TEXT("UBSPlayerUISubSystem::Get is  nullptr"));
	
	return nullptr;
}
// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFeatureAction_AddWidgets.h"

#include "BSLogChannels.h"
#include "Blueprint/UserWidget.h"
#include "Components/Widget.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFeatureAction.h"
#include "Engine/LocalPlayer.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

void UGameFeatureAction_AddWidgets::OnGameFeatureRegistering()
{
    Super::OnGameFeatureRegistering();
}

void UGameFeatureAction_AddWidgets::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
    Super::OnGameFeatureActivating(Context);

    if (!ensureAlways(Widgets.Num() > 0))
    {
        return;
    }

    FPerContextData& ActiveData = ContextData.FindOrAdd(Context);

    for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
    {
        if (Context.ShouldApplyToWorldContext(WorldContext))
        {
            if (UWorld* World = WorldContext.World())
            {
                for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
                {
                    if (APlayerController* PC = Iterator->Get())
                    {
                        if (ULocalPlayer* LP = Cast<ULocalPlayer>(PC->Player))
                        {
                            AddWidgetsForPlayer(LP, ActiveData);
                        }
                    }
                }
            }
        }
    }

    UE_LOG(LogBS, Warning, TEXT("UGameFeatureAction_AddWidgets::OnGameFeatureActivating"));
}

void UGameFeatureAction_AddWidgets::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
    Super::OnGameFeatureDeactivating(Context);

    if (FPerContextData* ActiveData = ContextData.Find(Context))
    {
        // Remove widgets from all players
        for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
        {
            if (Context.ShouldApplyToWorldContext(WorldContext))
            {
                if (UWorld* World = WorldContext.World())
                {
                    for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
                    {
                        if (APlayerController* PC = Iterator->Get())
                        {
                            if (ULocalPlayer* LP = Cast<ULocalPlayer>(PC->Player))
                            {
                                RemoveWidgetsForPlayer(LP, *ActiveData);
                            }
                        }
                    }
                }
            }
        }

        ContextData.Remove(Context);
    }
}

void UGameFeatureAction_AddWidgets::OnGameFeatureUnregistering()
{
    Super::OnGameFeatureUnregistering();
    
    // Clean up any remaining context data
    ContextData.Empty();
}

void UGameFeatureAction_AddWidgets::AddWidgetsForPlayer(UPlayer* Player, FPerContextData& ActiveData)
{
    ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
    if (!LocalPlayer)
    {
        return;
    }

    APlayerController* PlayerController = LocalPlayer->GetPlayerController(GetWorld());
    if (!PlayerController)
    {
        return;
    }

    // Load and create widgets
    TArray<FSoftObjectPath> WidgetClassPaths;
    for (const FGameFeatureWidgetEntry& Entry : Widgets)
    {
        if (!Entry.WidgetClass.IsNull())
        {
            WidgetClassPaths.Add(Entry.WidgetClass.ToSoftObjectPath());
        }
    }

    if (WidgetClassPaths.Num() > 0)
    {
        FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
        StreamableManager.RequestAsyncLoad(WidgetClassPaths, FStreamableDelegate::CreateUObject(
            this, &UGameFeatureAction_AddWidgets::OnWidgetClassesLoaded, Player, ActiveData));
    }
}

void UGameFeatureAction_AddWidgets::OnWidgetClassesLoaded(UPlayer* Player, FPerContextData ActiveData)
{
    ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
    if (!LocalPlayer)
    {
        return;
    }

    APlayerController* PlayerController = LocalPlayer->GetPlayerController(nullptr);
    if (!PlayerController)
    {
        return;
    }

    // Create and add widgets
    for (const FGameFeatureWidgetEntry& Entry : Widgets)
    {
        if (UClass* WidgetClass = Entry.WidgetClass.Get())
        {
            if (UUserWidget* Widget = CreateWidget<UUserWidget>(PlayerController, WidgetClass))
            {
                // Add to viewport or HUD slot based on your UI system
                if (Entry.SlotTag.IsValid())
                {
                    // If using Common UI or similar system with slots
                    // UCommonUIExtensions::AddWidgetToSlot(LocalPlayer, Widget, Entry.SlotTag, Entry.ZOrder);
                }
                else
                {
                    Widget->AddToViewport(Entry.ZOrder);
                }

                ActiveData.AddedWidgets.Add(Widget);
            }
        }
    }
}

void UGameFeatureAction_AddWidgets::RemoveWidgetsForPlayer(UPlayer* Player, FPerContextData& ActiveData)
{
    // Remove all added widgets
    for (TWeakObjectPtr<UUserWidget>& WidgetPtr : ActiveData.AddedWidgets)
    {
        if (UUserWidget* Widget = WidgetPtr.Get())
        {
            Widget->RemoveFromParent();
        }
    }
    
    ActiveData.AddedWidgets.Empty();
}


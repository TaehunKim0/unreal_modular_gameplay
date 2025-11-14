// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFeatureAction_AddWidgets.h"

#include "Etc/BSLogChannels.h"
#include "Blueprint/UserWidget.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFeatureAction.h"
#include "Engine/LocalPlayer.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "UI/SubSystem/BSPlayerUISubSystem.h"

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

    for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
    {
        if (Context.ShouldApplyToWorldContext(WorldContext))
        {
            if (UWorld* World = WorldContext.World())
            {
                for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
                {
                    for (auto WidgetEntry : Widgets)
                    {
                        if (APlayerController* PC = Iterator->Get())
                        {
                            auto TargetClass = WidgetEntry.TargetActorClass.LoadSynchronous();
                            auto Pawn = PC->GetPawn();
                            
                            if (Pawn && Pawn->IsA(TargetClass))
                            {
                                if (ULocalPlayer* LP = Cast<ULocalPlayer>(PC->Player))
                                {
                                    AddWidgetsForPlayer(LP);
                                }
                            }
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
    
    while (!AddedWidgets.IsEmpty())
    {
        auto It = AddedWidgets.CreateIterator();
        auto LocalPlayer = It->Key;
        if (IsValid(LocalPlayer) && LocalPlayer->IsValidLowLevel())
        {
            RemoveWidgetsForPlayer(LocalPlayer);
            continue;
        }

        It.RemoveCurrent();
    }
    
    AddedWidgets.Reset();
}

void UGameFeatureAction_AddWidgets::OnGameFeatureUnregistering()
{
    Super::OnGameFeatureUnregistering();
}

void UGameFeatureAction_AddWidgets::AddWidgetsForPlayer(UPlayer* InPlayer)
{
    ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(InPlayer);
    if (!LocalPlayer)
    {
        return;
    }

    APlayerController* PlayerController = LocalPlayer->GetPlayerController(GetWorld());
    if (!PlayerController)
    {
        return;
    }

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
            this, &UGameFeatureAction_AddWidgets::OnWidgetClassesLoaded, InPlayer));
    }
}

void UGameFeatureAction_AddWidgets::OnWidgetClassesLoaded(UPlayer* InPlayer)
{
    ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(InPlayer);
    if (!LocalPlayer)
    {
        return;
    }

    APlayerController* PlayerController = LocalPlayer->GetPlayerController(GetWorld());
    if (!PlayerController)
    {
        return;
    }

    for (const FGameFeatureWidgetEntry& Entry : Widgets)
    {
        if (const TSubclassOf<UUserWidget> WidgetClass = Entry.WidgetClass.Get())
        {
            auto ResultWidget = UBSPlayerUISubSystem::Get(PlayerController)->CreateWidget<UUserWidget>(WidgetClass, Entry.WidgetCategory, PlayerController);
            if (!ResultWidget)
            {
                UE_LOG(LogBS, Error, TEXT("UGameFeatureAction_AddWidgets::Cannot CreateWidget"));
            }
            
            auto& Category = AddedWidgets.FindOrAdd(LocalPlayer);
            Category = Entry.WidgetCategory;
        }
    }
}

void UGameFeatureAction_AddWidgets::RemoveWidgetsForPlayer(UPlayer* InPlayer)
{
    ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(InPlayer);
    if (!LocalPlayer)
    {
        return;
    }
    
    APlayerController* PlayerController = LocalPlayer->GetPlayerController(GetWorld());
    if (!PlayerController)
    {
        return;
    }
    
    auto Category = AddedWidgets.Find(LocalPlayer);

    if (UBSPlayerUISubSystem::Get(PlayerController))
        UBSPlayerUISubSystem::Get(PlayerController)->RemoveWidget(*Category);

    AddedWidgets.Remove(LocalPlayer);
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "BSPlayerController.h"

#include "BSLogChannels.h"
#include "GameModes/BSGameState.h"
#include "UI/SubSystem/BSPlayerUISubSystem.h"

ABSPlayerController::ABSPlayerController()
{
	UE_LOG(LogBS, Log, TEXT("ABSPlayerController::ABSPlayerController"));
}

void ABSPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UE_LOG(LogBS, Log, TEXT("ABSPlayerController::OnPossess"));
}

void ABSPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ABSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (auto GameState = Cast<ABSGameState>(GetWorld()->GetGameState()))
	{
		UE_LOG(LogBS, Log, TEXT("ABSPlayerController::CreateWidget"));
		auto Widget = UBSPlayerUISubSystem::Get(this)->CreateWidget(GameState->CharacterSelectionWidgetClass,
			EUICategory::CharacterSelection, this);
		
		FInputModeUIOnly InputModeUI;
		InputModeUI.SetWidgetToFocus(Widget->TakeWidget());
		InputModeUI.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputModeUI);
		bShowMouseCursor = true;
	}
}

void ABSPlayerController::CheckGameFeatureStatus()
{
	FString PluginURL = TEXT("file:../../../../../../Users/jack0/GitHub/Personal/bishoujo_doom/Bishoujo_Doom/Plugins/GameFeatures/Characters/Ellie/Ellie.uplugin");
    
	UGameFeaturesSubsystem& GFS = UGameFeaturesSubsystem::Get();
    
	bool IsLoaded = GFS.IsGameFeaturePluginLoaded(PluginURL);
    
	FString RoleString;
	if (GetNetMode() == NM_Client)
	{
		RoleString = TEXT("Client");
	}
	else if (GetNetMode() == NM_ListenServer)
	{
		RoleString = TEXT("ListenServer");
	}
	else if (GetNetMode() == NM_DedicatedServer)
	{
		RoleString = TEXT("DedicatedServer");
	}
	else
	{
		RoleString = TEXT("Standalone");
	}
    
	UE_LOG(LogBS, Warning, TEXT("=== GameFeature Status ==="));
	UE_LOG(LogBS, Warning, TEXT("Role: %s"), *RoleString);
	UE_LOG(LogBS, Warning, TEXT("IsLoaded: %s"), IsLoaded ? TEXT("true") : TEXT("false"));
	UE_LOG(LogBS, Warning, TEXT("IsLocalController: %s"), IsLocalController() ? TEXT("true") : TEXT("false"));
    
	if (IsLocalController())
	{
		UBSPlayerUISubSystem::Get(this)->ShowDebugMessage("Role", RoleString);
		UBSPlayerUISubSystem::Get(this)->ShowDebugMessage("Loaded", IsLoaded ? TEXT("Yes") : TEXT("No"));
	}
}
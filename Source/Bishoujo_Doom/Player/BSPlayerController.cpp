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

void ABSPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

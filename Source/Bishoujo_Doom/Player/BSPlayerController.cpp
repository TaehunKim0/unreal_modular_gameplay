// Fill out your copyright notice in the Description page of Project Settings.


#include "BSPlayerController.h"

#include "BSLogChannels.h"

ABSPlayerController::ABSPlayerController()
{
	UE_LOG(LogBS, Log, TEXT("ABSPlayerController::ABSPlayerController"));
}

void ABSPlayerController::OnPossess(APawn* InPawn)
{
	UE_LOG(LogBS, Log, TEXT("ABSPlayerController::OnPossess"));
	
	Super::OnPossess(InPawn);
}

void ABSPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "BSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BISHOUJO_DOOM_API ABSPlayerController : public AModularPlayerController
{
	GENERATED_BODY()

public:
	ABSPlayerController();

	virtual void OnPossess(APawn* InPawn) override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

	UFUNCTION(Exec)
	void CheckGameFeatureStatus();
};
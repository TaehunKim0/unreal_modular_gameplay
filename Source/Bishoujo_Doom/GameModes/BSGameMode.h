// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularGameMode.h"
#include "GameFramework/GameModeBase.h"
#include "BSGameMode.generated.h"

class UBSCharacterDefinition;
class ABSPlayerState;
class UBSAssetManager;
class UBSPawnData;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class BISHOUJO_DOOM_API ABSGameMode : public AModularGameModeBase
{
	GENERATED_BODY()

public:
	ABSGameMode();

protected:
	//~AGameModeBase interface
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void StartPlay() override;
	virtual void BeginPlay() override;
	//~End of AGameModeBase interface

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameMode)
	UBSCharacterDefinition* TestDef;
};
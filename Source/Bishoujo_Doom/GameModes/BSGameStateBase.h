// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularGameState.h"
#include "BSGameStateBase.generated.h"

struct FStreamableHandle;
class UBSCharacterDefinition;

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class BISHOUJO_DOOM_API ABSGameStateBase : public AModularGameStateBase
{
	GENERATED_BODY()
public:
	ABSGameStateBase();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	// 현재 캐릭터 정의 가져오기
	UFUNCTION(BlueprintPure, Category = "Character")
	UBSCharacterDefinition* GetDefaultCharacterDefinition() const { return DefaultCharacterDefinition; }
	
	// Default character definition for this game mode/map
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Game Settings")
	UBSCharacterDefinition* DefaultCharacterDefinition;
};
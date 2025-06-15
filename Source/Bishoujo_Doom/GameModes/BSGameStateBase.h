// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BSGameStateBase.generated.h"

struct FStreamableHandle;
class UBSCharacterDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterDefinitionLoaded, UBSCharacterDefinition*, CharacterDefinition);
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class BISHOUJO_DOOM_API ABSGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
public:
	ABSGameStateBase();

protected:
	virtual void BeginPlay() override;
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
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "Subsystems/EngineSubsystem.h"
#include "BSAssetManager.generated.h"

class UBSCharacterDefinition;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnBSCharacterLoaded, const UBSCharacterDefinition* /*Experience*/);
/**
 * 
 */
UCLASS()
class BISHOUJO_DOOM_API UBSAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	static UBSAssetManager& Get();
    
	// 캐릭터 관련
	void LoadCharacterDefinition(const FPrimaryAssetId& InCharacterDefinitionId,
								const FStreamableDelegate& InLoadCompleteDelegate);

private:
	FOnBSCharacterLoaded OnCharacterLoaded;
};
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFeaturesSubsystem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BSGameFeatureSystem.generated.h"

class ABSPlayerState;
/**
 * 
 */
UCLASS()
class BISHOUJO_DOOM_API UBSGameFeatureSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:	
	virtual void BeginDestroy() override;


public:
	void EnableGameFeature(
		const FString& GameFeatureNameToEnable,
		const FGameFeaturePluginLoadComplete& LoadCompleteDelegate,
		const FGameFeaturePluginLoadComplete& ActiveCompleteDelegate);
	
	void DisableGameFeatures(const TArray<FString>& GameFeaturesToDisable);
	void DisableAllGameFeatures();

	FString GetPluginURLByName(FString InFeatureName) const;

private:
	void OnGameFeaturePluginDeactivateComplete(const UE::GameFeatures::FResult& Result);

private:
	FGameFeaturePluginDeactivateComplete OnPluginDeactivateCompleteDelegate;
	FGameFeaturePluginUnloadComplete OnPluginUnloadCompleteDelegate;
};

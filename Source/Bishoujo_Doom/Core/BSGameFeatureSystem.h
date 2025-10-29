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

	UFUNCTION(BlueprintCallable)
	void K2_EnableGameFeature(const FString& GameFeatureNameToEnable);

public:
	void DisableGameFeature(const FString& GameFeatureToDisable);
	void DisableGameFeatures(const TArray<FString>& GameFeaturesToDisable);
	void DisableAllGameFeatures();

	UFUNCTION(BlueprintCallable)
	void K2_DisableGameFeature(const FString& GameFeatureNameToEnable);

public:
	void UnLoadGameFeature(const FString& GameFeatureToUnLoad);
	UFUNCTION(BlueprintCallable)
	void K2_UnLoadGameFeature(const FString& GameFeatureToUnLoad);
	
public:
	void ReleaseGameFeature(const FString& GameFeatureToRelease);
	UFUNCTION(BlueprintCallable)
	void K2_ReleaseGameFeature(const FString& GameFeatureToRelease);
	
public:
	bool IsGameFeatureActive(const FString& InGameFeatureName);
	UFUNCTION(BlueprintCallable)
	bool K2_IsGameFeatureActive(const FString& InGameFeatureName);

public:
	FString GetPluginURLByName(FString InFeatureName) const;

private:
	void OnGameFeaturePluginDeactivateComplete(const UE::GameFeatures::FResult& Result);

private:
	FGameFeaturePluginDeactivateComplete OnPluginDeactivateCompleteDelegate;
	FGameFeaturePluginUnloadComplete OnPluginUnloadCompleteDelegate;
};

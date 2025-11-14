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
		const FString& InGameFeatureNameToEnable,
		const FGameFeaturePluginLoadComplete& InLoadCompleteDelegate,
		const FGameFeaturePluginLoadComplete& InActiveCompleteDelegate);

	UFUNCTION(BlueprintCallable)
	void K2_EnableGameFeature(const FString& InGameFeatureNameToEnable);

public:
	void DisableGameFeature(const FString& InGameFeatureToDisable);
	void DisableGameFeatures(const TArray<FString>& InGameFeaturesToDisable);
	void DisableAllGameFeatures();

	UFUNCTION(BlueprintCallable)
	void K2_DisableGameFeature(const FString& InGameFeatureNameToEnable);

public:
	void UnLoadGameFeature(const FString& InGameFeatureToUnLoad);
	UFUNCTION(BlueprintCallable)
	void K2_UnLoadGameFeature(const FString& InGameFeatureToUnLoad);
	
public:
	void ReleaseGameFeature(const FString& InGameFeatureToRelease);
	UFUNCTION(BlueprintCallable)
	void K2_ReleaseGameFeature(const FString& InGameFeatureToRelease);
	
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

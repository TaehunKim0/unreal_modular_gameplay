// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/BSGameFeatureSystem.h"

#include "BSLogChannels.h"
#include "GameFeatureData.h"


void UBSGameFeatureSystem::BeginDestroy()
{
	Super::BeginDestroy();

	DisableAllGameFeatures();
}

void UBSGameFeatureSystem::EnableGameFeature(
	const FString& GameFeatureNameToEnable,
	const FGameFeaturePluginLoadComplete& LoadCompleteDelegate,
	const FGameFeaturePluginLoadComplete& ActiveCompleteDelegate)
{
	UGameFeaturesSubsystem& GameFeatureSubsystem = UGameFeaturesSubsystem::Get();

	GameFeatureSubsystem.LoadGameFeaturePlugin(
		GetPluginURLByName(GameFeatureNameToEnable),
		LoadCompleteDelegate);

	GameFeatureSubsystem.LoadAndActivateGameFeaturePlugin(
		GetPluginURLByName(GameFeatureNameToEnable),
		ActiveCompleteDelegate);

}

void UBSGameFeatureSystem::DisableGameFeatures(const TArray<FString>& GameFeaturesToDisable)
{
	UGameFeaturesSubsystem* GameFeatureSubsystem = GEngine->GetEngineSubsystem<UGameFeaturesSubsystem>();
	if (!IsValid(GameFeatureSubsystem))
	{
		UE_LOG(LogBS, Error, TEXT("UBSGameFeatureSystem::GameFeatureSubsystem is not valid"));
	}

	for (const FString& FeatureName : GameFeaturesToDisable)
	{
		FString PluginURL;
		UGameFeaturesSubsystem::Get().GetPluginURLByName(FeatureName, PluginURL);

		UE_LOG(LogBS, Warning, TEXT("UBSGameFeatureSystem::Deactivating GameFeature: %s"), *FeatureName);
		
		OnPluginDeactivateCompleteDelegate.BindUObject(this, &UBSGameFeatureSystem::OnGameFeaturePluginDeactivateComplete);
		GameFeatureSubsystem->DeactivateGameFeaturePlugin(PluginURL, OnPluginDeactivateCompleteDelegate);
	}
}

void UBSGameFeatureSystem::DisableAllGameFeatures()
{
	UGameFeaturesSubsystem* GameFeatureSubsystem = GEngine->GetEngineSubsystem<UGameFeaturesSubsystem>();
	if (!GameFeatureSubsystem)
		return;
	
	TArray<FString> PluginNames;
	TArray<const UGameFeatureData*> ActivePluginsArray;
	
	GameFeatureSubsystem->GetGameFeatureDataForActivePlugins(ActivePluginsArray);
	for (const auto PluginData : ActivePluginsArray)
	{
		FString PluginName;
		PluginData->GetPluginName(PluginName);
		PluginNames.Add(PluginName);
	}

	DisableGameFeatures(PluginNames);
}

FString UBSGameFeatureSystem::GetPluginURLByName(FString InFeatureName) const
{
	FString PluginURL;
	UGameFeaturesSubsystem::Get().GetPluginURLByName(InFeatureName, PluginURL);

	return PluginURL;
}

void UBSGameFeatureSystem::OnGameFeaturePluginDeactivateComplete(const UE::GameFeatures::FResult& Result)
{
	UE_LOG(LogBS, Warning, TEXT("UBSGameFeatureSystem::GameFeatureDeactivateComplete"));

	
	// GameFeatureSubsystem->UnloadGameFeaturePlugin(PluginURL, OnPluginUnloadCompleteDelegate);
	// TODO : 일회성 기능이나 메모리 비용이 큰 피처들만 Unload ,
}

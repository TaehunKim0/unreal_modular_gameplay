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

void UBSGameFeatureSystem::K2_EnableGameFeature(const FString& GameFeatureNameToEnable)
{
	EnableGameFeature(
		GameFeatureNameToEnable,
		FGameFeaturePluginLoadComplete{},
		FGameFeaturePluginLoadComplete::CreateLambda([GameFeatureNameToEnable](const UE::GameFeatures::FResult& Result)
		{
			UE_LOG(LogBS, Warning, TEXT("UBSGameFeatureSystem::GameFeatureSubsystem Activated : %s"), *GameFeatureNameToEnable);
		}));
}

void UBSGameFeatureSystem::DisableGameFeature(const FString& GameFeatureToDisable)
{
	UGameFeaturesSubsystem* GameFeatureSubsystem = GEngine->GetEngineSubsystem<UGameFeaturesSubsystem>();
	if (!IsValid(GameFeatureSubsystem))
	{
		UE_LOG(LogBS, Error, TEXT("UBSGameFeatureSystem::GameFeatureSubsystem is not valid"));
	}
	
	FString PluginURL;
	UGameFeaturesSubsystem::Get().GetPluginURLByName(GameFeatureToDisable, PluginURL);

	UE_LOG(LogBS, Warning, TEXT("UBSGameFeatureSystem::Deactivating GameFeature: %s"), *GameFeatureToDisable);
	
	OnPluginDeactivateCompleteDelegate.BindUObject(this, &UBSGameFeatureSystem::OnGameFeaturePluginDeactivateComplete);
	GameFeatureSubsystem->DeactivateGameFeaturePlugin(PluginURL, OnPluginDeactivateCompleteDelegate);
}

void UBSGameFeatureSystem::DisableGameFeatures(const TArray<FString>& GameFeaturesToDisable)
{
	for (const FString& FeatureName : GameFeaturesToDisable)
	{
		DisableGameFeature(FeatureName);
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

void UBSGameFeatureSystem::K2_DisableGameFeature(const FString& GameFeatureNameToEnable)
{
	DisableGameFeature(GameFeatureNameToEnable);
}

void UBSGameFeatureSystem::UnLoadGameFeature(const FString& GameFeatureToUnLoad)
{
	UGameFeaturesSubsystem* GameFeatureSubsystem = GEngine->GetEngineSubsystem<UGameFeaturesSubsystem>();
	if (!GameFeatureSubsystem)
	{
		UE_LOG(LogBS, Error, TEXT("UBSGameFeatureSystem Get is Failed"));
		return;
	}

	GameFeatureSubsystem->UnloadGameFeaturePlugin(
		GetPluginURLByName(GameFeatureToUnLoad),
		FGameFeaturePluginLoadComplete::CreateLambda([GameFeatureToUnLoad](const UE::GameFeatures::FResult& Result)
		{
			UE_LOG(LogBS, Warning, TEXT("UBSGameFeatureSystem::GameFeatureSubsystem Unload : %s"), *GameFeatureToUnLoad);
	}));
}

void UBSGameFeatureSystem::K2_UnLoadGameFeature(const FString& GameFeatureToUnLoad)
{
	UnLoadGameFeature(GameFeatureToUnLoad);
}

void UBSGameFeatureSystem::ReleaseGameFeature(const FString& GameFeatureToRelease)
{
	UGameFeaturesSubsystem* GameFeatureSubsystem = GEngine->GetEngineSubsystem<UGameFeaturesSubsystem>();
	if (!GameFeatureSubsystem)
	{
		UE_LOG(LogBS, Error, TEXT("UBSGameFeatureSystem Get is Failed"));
		return;
	}

	GameFeatureSubsystem->ReleaseGameFeaturePlugin(
		GetPluginURLByName(GameFeatureToRelease),
		FGameFeaturePluginReleaseComplete::CreateLambda([GameFeatureToRelease](const UE::GameFeatures::FResult& Result)
		{
			UE_LOG(LogBS, Warning, TEXT("UBSGameFeatureSystem::GameFeatureSubsystem Released : %s"), *GameFeatureToRelease);
		}));
}

void UBSGameFeatureSystem::K2_ReleaseGameFeature(const FString& GameFeatureToRelease)
{
	ReleaseGameFeature(GameFeatureToRelease);
}

bool UBSGameFeatureSystem::K2_IsGameFeatureActive(const FString& InGameFeatureName)
{
	return IsGameFeatureActive(InGameFeatureName);	
}

bool UBSGameFeatureSystem::IsGameFeatureActive(const FString& InGameFeatureName)
{
	UGameFeaturesSubsystem* GameFeatureSubsystem = GEngine->GetEngineSubsystem<UGameFeaturesSubsystem>();
	if (!GameFeatureSubsystem)
	{
		UE_LOG(LogBS, Error, TEXT("UBSGameFeatureSystem Get is Failed"));
		return false;
	}

	return GameFeatureSubsystem->IsGameFeaturePluginActive(GetPluginURLByName(InGameFeatureName));
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

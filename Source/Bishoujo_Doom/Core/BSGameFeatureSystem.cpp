// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/BSGameFeatureSystem.h"

#include "Etc/BSLogChannels.h"
#include "GameFeatureData.h"


void UBSGameFeatureSystem::BeginDestroy()
{
	Super::BeginDestroy();

	DisableAllGameFeatures();
}

void UBSGameFeatureSystem::EnableGameFeature(
	const FString& InGameFeatureNameToEnable,
	const FGameFeaturePluginLoadComplete& InLoadCompleteDelegate,
	const FGameFeaturePluginLoadComplete& InActiveCompleteDelegate)
{
	UGameFeaturesSubsystem& GameFeatureSubsystem = UGameFeaturesSubsystem::Get();

	
	GameFeatureSubsystem.LoadGameFeaturePlugin(
		GetPluginURLByName(InGameFeatureNameToEnable),
		InLoadCompleteDelegate);

	GameFeatureSubsystem.LoadAndActivateGameFeaturePlugin(
		GetPluginURLByName(InGameFeatureNameToEnable),
		InActiveCompleteDelegate);
}

void UBSGameFeatureSystem::K2_EnableGameFeature(const FString& InGameFeatureNameToEnable)
{
	EnableGameFeature(
		InGameFeatureNameToEnable,
		FGameFeaturePluginLoadComplete{},
		FGameFeaturePluginLoadComplete::CreateLambda([InGameFeatureNameToEnable](const UE::GameFeatures::FResult& Result)
		{
			UE_LOG(LogBS, Warning, TEXT("UBSGameFeatureSystem::GameFeatureSubsystem Activated : %s"), *InGameFeatureNameToEnable);
		}));
}

void UBSGameFeatureSystem::DisableGameFeature(const FString& InGameFeatureToDisable)
{
	UGameFeaturesSubsystem* GameFeatureSubsystem = GEngine->GetEngineSubsystem<UGameFeaturesSubsystem>();
	if (!IsValid(GameFeatureSubsystem))
	{
		UE_LOG(LogBS, Error, TEXT("UBSGameFeatureSystem::GameFeatureSubsystem is not valid"));
	}
	
	FString PluginURL;
	UGameFeaturesSubsystem::Get().GetPluginURLByName(InGameFeatureToDisable, PluginURL);

	UE_LOG(LogBS, Warning, TEXT("UBSGameFeatureSystem::Deactivating GameFeature: %s"), *InGameFeatureToDisable);
	
	OnPluginDeactivateCompleteDelegate.BindUObject(this, &UBSGameFeatureSystem::OnGameFeaturePluginDeactivateComplete);
	GameFeatureSubsystem->DeactivateGameFeaturePlugin(PluginURL, OnPluginDeactivateCompleteDelegate);
}

void UBSGameFeatureSystem::DisableGameFeatures(const TArray<FString>& InGameFeaturesToDisable)
{
	for (const FString& FeatureName : InGameFeaturesToDisable)
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

void UBSGameFeatureSystem::K2_DisableGameFeature(const FString& InGameFeatureNameToEnable)
{
	DisableGameFeature(InGameFeatureNameToEnable);
}

void UBSGameFeatureSystem::UnLoadGameFeature(const FString& InGameFeatureToUnLoad)
{
	UGameFeaturesSubsystem* GameFeatureSubsystem = GEngine->GetEngineSubsystem<UGameFeaturesSubsystem>();
	if (!GameFeatureSubsystem)
	{
		UE_LOG(LogBS, Error, TEXT("UBSGameFeatureSystem Get is Failed"));
		return;
	}

	GameFeatureSubsystem->UnloadGameFeaturePlugin(
		GetPluginURLByName(InGameFeatureToUnLoad),
		FGameFeaturePluginLoadComplete::CreateLambda([InGameFeatureToUnLoad](const UE::GameFeatures::FResult& Result)
		{
			UE_LOG(LogBS, Warning, TEXT("UBSGameFeatureSystem::GameFeatureSubsystem Unload : %s"), *InGameFeatureToUnLoad);
	}));
}

void UBSGameFeatureSystem::K2_UnLoadGameFeature(const FString& InGameFeatureToUnLoad)
{
	UnLoadGameFeature(InGameFeatureToUnLoad);
}

void UBSGameFeatureSystem::ReleaseGameFeature(const FString& InGameFeatureToRelease)
{
	UGameFeaturesSubsystem* GameFeatureSubsystem = GEngine->GetEngineSubsystem<UGameFeaturesSubsystem>();
	if (!GameFeatureSubsystem)
	{
		UE_LOG(LogBS, Error, TEXT("UBSGameFeatureSystem Get is Failed"));
		return;
	}

	GameFeatureSubsystem->ReleaseGameFeaturePlugin(
		GetPluginURLByName(InGameFeatureToRelease),
		FGameFeaturePluginReleaseComplete::CreateLambda([InGameFeatureToRelease](const UE::GameFeatures::FResult& Result)
		{
			UE_LOG(LogBS, Warning, TEXT("UBSGameFeatureSystem::GameFeatureSubsystem Released : %s"), *InGameFeatureToRelease);
		}));
}

void UBSGameFeatureSystem::K2_ReleaseGameFeature(const FString& InGameFeatureToRelease)
{
	ReleaseGameFeature(InGameFeatureToRelease);
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

// Fill out your copyright notice in the Description page of Project Settings.


#include "BSGameInstance.h"

#include "Etc/BSGamePlayTags.h"
#include "Components/GameFrameworkComponentManager.h"

void UBSGameInstance::Init()
{
	Super::Init();

	UE_LOG(LogBS, Log, TEXT("BSGameInstance::Init"));

	UGameFrameworkComponentManager* ComponentManager = GetSubsystem<UGameFrameworkComponentManager>(this);

	if (ensure(ComponentManager))
	{
		ComponentManager->RegisterInitState(BSGamePlayTags::InitState_Spawned, false, FGameplayTag());
		ComponentManager->RegisterInitState(BSGamePlayTags::InitState_DataAvailable, false, FGameplayTag());
		ComponentManager->RegisterInitState(BSGamePlayTags::InitState_DataInitialized, false, FGameplayTag());
		ComponentManager->RegisterInitState(BSGamePlayTags::InitState_GameplayReady, false, FGameplayTag());
	}
}

void UBSGameInstance::StartGameInstance()
{
	Super::StartGameInstance();
	UE_LOG(LogBS, Log, TEXT("BSGameInstance::StartGameInstance"));
}

void UBSGameInstance::Shutdown()
{
	Super::Shutdown();
	UE_LOG(LogBS, Log, TEXT("BSGameInstance::Shutdown"));
}

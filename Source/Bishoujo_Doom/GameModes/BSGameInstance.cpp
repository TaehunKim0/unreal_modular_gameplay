// Fill out your copyright notice in the Description page of Project Settings.


#include "BSGameInstance.h"

#include "BSGamePlayTags.h"
#include "Components/GameFrameworkComponentManager.h"

void UBSGameInstance::Init()
{
	Super::Init();

	UE_LOG(LogBS, Log, TEXT("BSGameInstance::Init"));

	UGameFrameworkComponentManager* ComponentManager = GetSubsystem<UGameFrameworkComponentManager>(this);

	if (ensure(ComponentManager))
	{
		ComponentManager->RegisterInitState(BSGamePlayTags::InitState_Spawned, false, FGameplayTag());
		ComponentManager->RegisterInitState(BSGamePlayTags::InitState_PlayerStateInitialized, false, FGameplayTag());
		ComponentManager->RegisterInitState(BSGamePlayTags::InitState_ASCInitialized, false, FGameplayTag());
		ComponentManager->RegisterInitState(BSGamePlayTags::InitState_CharacterDefinitionInitialized, false, FGameplayTag());
		ComponentManager->RegisterInitState(BSGamePlayTags::InitState_CharacterComponentInitialized, false, FGameplayTag());
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

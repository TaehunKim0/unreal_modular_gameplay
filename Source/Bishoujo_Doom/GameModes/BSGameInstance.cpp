// Fill out your copyright notice in the Description page of Project Settings.


#include "BSGameInstance.h"

#include "BSGamePlayTags.h"
#include "Components/GameFrameworkComponentManager.h"

void UBSGameInstance::Init()
{
	Super::Init();

	UGameFrameworkComponentManager* ComponentManager = GetSubsystem<UGameFrameworkComponentManager>(this);

	if (ensure(ComponentManager))
	{
		ComponentManager->RegisterInitState(BSGamePlayTags::InitState_Spawned, false, FGameplayTag());
		ComponentManager->RegisterInitState(BSGamePlayTags::InitState_DataAvailable, false, BSGamePlayTags::InitState_Spawned);
		ComponentManager->RegisterInitState(BSGamePlayTags::InitState_DataInitialized, false, BSGamePlayTags::InitState_DataAvailable);
		ComponentManager->RegisterInitState(BSGamePlayTags::InitState_GameplayReady, false, BSGamePlayTags::InitState_DataInitialized);
	}
}

void UBSGameInstance::Shutdown()
{
	Super::Shutdown();
}

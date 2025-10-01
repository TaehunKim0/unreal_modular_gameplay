// Fill out your copyright notice in the Description page of Project Settings.


#include "BSAssetManager.h"

#include "Core/BSCharacterDefinition.h"
#include "Engine/AssetManager.h"
#include "Engine/Engine.h"

UBSAssetManager& UBSAssetManager::Get()
{
	check(GEngine);
	if (UBSAssetManager* Singleton = Cast<UBSAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}
    
	UE_LOG(LogTemp, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini"));
	return *NewObject<UBSAssetManager>(); // Fatal이라 여기 도달 안함
}

void UBSAssetManager::LoadCharacterDefinition(const FPrimaryAssetId& InCharacterDefinitionId,
                                              const FStreamableDelegate& InLoadCompleteDelegate)
{
	// 기본 에셋 경로 가져오기
	const FSoftObjectPath AssetPath = GetPrimaryAssetPath(InCharacterDefinitionId);
	if (!AssetPath.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid asset path for: %s"), *InCharacterDefinitionId.ToString());
		InLoadCompleteDelegate.ExecuteIfBound();
		return;
	}
    
	// 로드할 에셋 목록 구성
	TArray<FSoftObjectPath> AssetsToLoad;
	AssetsToLoad.Add(AssetPath);
    
	// 비동기 로딩 시작
	LoadAssetList(AssetsToLoad, InLoadCompleteDelegate, FStreamableManager::AsyncLoadHighPriority);
}
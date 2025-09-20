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

void UBSAssetManager::LoadCharacterDefinition(FPrimaryAssetId CharacterDefinitionId, 
                                              FStreamableDelegate LoadComplete)
{
	LoadPrimaryAssetWithDependencies<UBSCharacterDefinition>(CharacterDefinitionId, LoadComplete);
}

template<typename AssetType>
void UBSAssetManager::LoadPrimaryAssetWithDependencies(FPrimaryAssetId AssetId, 
													  FStreamableDelegate LoadComplete)
{
	// 기본 에셋 경로 가져오기
	FSoftObjectPath AssetPath = GetPrimaryAssetPath(AssetId);
	if (!AssetPath.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid asset path for: %s"), *AssetId.ToString());
		LoadComplete.ExecuteIfBound();
		return;
	}
    
	// 의존성 포함해서 로드할 에셋 목록 구성
	TArray<FSoftObjectPath> AssetsToLoad;
	AssetsToLoad.Add(AssetPath);
    
	// 비동기 로딩 시작
	LoadAssetList(AssetsToLoad, LoadComplete, FStreamableManager::AsyncLoadHighPriority);
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/BSCharacterDefManagerComponent.h"

#include "BSCharacterDefinition.h"
#include "BSLogChannels.h"
#include "GameFeatureAction.h"
#include "GameFeaturesSubsystem.h"
#include "AbilitySystem/BSAbilitySet.h"
#include "Character/BSPawnData.h"
#include "Engine/AssetManager.h"
#include "GameModes/BSGameState.h"
#include "Player/BSPlayerState.h"
#include "UI/SubSystem/BSPlayerUISubSystem.h"

UBSCharacterDefManagerComponent::UBSCharacterDefManagerComponent(const FObjectInitializer& ObjectInitializer)
	:  Super(ObjectInitializer)
{
	OnCharacterDefinitionChangedDelegate.AddDynamic(this, &UBSCharacterDefManagerComponent::OnCharacterDefinitionChanged);
}

void UBSCharacterDefManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogBS, Log, TEXT("UBSCharacterDefManagerComponent::BeginPlay"));
}

void UBSCharacterDefManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	DisableAllGameFeatures();
}

void UBSCharacterDefManagerComponent::SetCharacterDefinition(APlayerState* InPlayerState,
                                                             const FGameplayTag InTag)
{
	TSoftObjectPtr<const UBSCharacterDefinition>* NewCharacterDef = nullptr;
	NewCharacterDef = GetGameState<ABSGameState>()->AvailableCharacterDefinitionMap.Find(InTag);
	
	if (!NewCharacterDef)
	{
		UE_LOG(LogBS, Warning, TEXT("Invalid Character Tag"));
		return;
	}
	
	if (!InPlayerState || !NewCharacterDef)
	{
		UE_LOG(LogBS, Warning, TEXT("Invalid parameters for SetCharacterDefinition"));
		return;
	}

	if (GetOwner()->GetLocalRole() != ROLE_Authority)
	{
		UE_LOG(LogBS, Warning, TEXT("SetCharacterDefinition can only be called on server"));
		return;
	}

	// 로드할 에셋들의 경로를 수집
	FSoftObjectPath AssetsToLoad = NewCharacterDef->ToSoftObjectPath();
	UAssetManager& AssetManager = UAssetManager::Get();
	
	AssetManager.GetStreamableManager().LoadSynchronous<UBSCharacterDefinition>(AssetsToLoad);
	// 비동기 로딩 시작
	auto AllAssetsLoadHandle = AssetManager.GetStreamableManager().RequestAsyncLoad(
		AssetsToLoad,
		FStreamableDelegate::CreateLambda([this, InPlayerState, NewCharacterDef]()
		{
			// 비동기 로딩 완료 후 실행
			UE_LOG(LogBS, Log, TEXT("Character assets loaded for player: %s"), *InPlayerState->GetPlayerName());

			// 에셋 로드 확인
			const UBSCharacterDefinition* LoadedCharacterDef = NewCharacterDef->Get();
			if (!LoadedCharacterDef)
			{
				UE_LOG(LogBS, Error, TEXT("Failed to load Character Definition"));
				return;
			}

			// 이전 캐릭터 정의 가져오기
			if (const auto PlayerState = Cast<ABSPlayerState>(InPlayerState))
			{
				const UBSCharacterDefinition* OldCharacterDef = PlayerState->GetCharacterDefData();

				// 같은 캐릭터 정의라면 무시
				if (OldCharacterDef == NewCharacterDef->Get())
				{
					UE_LOG(LogBS, Warning, TEXT("OldCharacterDef == NewCharacterDef"));
					return;
				}

				UE_LOG(LogBS, Log, TEXT("UBSCharacterDefManagerComponent:: Changing character definition for player: %s"), *PlayerState->GetPlayerName());

				// 이전 캐릭터 정의 정리
				if (OldCharacterDef)
				{
					CleanupCharacterDefinition(PlayerState, OldCharacterDef);
				}

				// 새 캐릭터 정의 적용
				PlayerState->SetCharacterDefData(NewCharacterDef->Get());
				ApplyCharacterDefinition(PlayerState, NewCharacterDef->Get());
			}

			// // 현재 Pawn이 있다면 재스폰 고려
			// if (APawn* CurrentPawn = InPlayerState->GetPawn())
			// {
			// 	// 새로운 PawnData가 다른 클래스를 사용한다면 재스폰 필요
			// 	const UBSPawnData* NewPawnData = NewCharacterDef->DefaultPawnData;
			// 	if (NewPawnData && NewPawnData->PawnClass != CurrentPawn->GetClass())
			// 	{
			// 		// 재스폰 로직 (GameMode에 위임)
			// 		if (AGameModeBase* GameMode = GetWorld()->GetAuthGameMode())
			// 		{
			// 			if (AController* Controller = InPlayerState->GetOwner<AController>())
			// 			{
			// 				// 현재 Pawn 제거 후 새로 스폰
			// 				CurrentPawn->Destroy();
			// 				GameMode->RestartPlayer(Controller);
			// 			}
			// 		}
			// 	}
			// }
		})
	);
}

void UBSCharacterDefManagerComponent::CleanupCharacterDefinition(ABSPlayerState* PlayerState,
	const UBSCharacterDefinition* OldCharacterDef)
{
	if (!OldCharacterDef) return;

	UE_LOG(LogBS, Log, TEXT("Cleaning up character definition"));

	// GameFeatures 비활성화
	DisableGameFeatures(PlayerState, OldCharacterDef->GameFeaturesToEnable);

	// AbilitySets 제거 (현재 Pawn이 있다면)
	if (APawn* CurrentPawn = PlayerState->GetPawn())
	{
		// AbilitySystemComponent에서 어빌리티들 제거
		// 구체적인 구현은 ASC 시스템에 따라 다름
	}
}

void UBSCharacterDefManagerComponent::CollectGameFeaturePluginURL(const TArray<FString>& InFeaturePluginURLArray)
{
	GameFeaturePluginURLs.Reset();
	
	for (const FString& PluginName : InFeaturePluginURLArray)
	{
		FString PluginURL;
		if (UGameFeaturesSubsystem::Get().GetPluginURLByName(PluginName, PluginURL))
		{
			GameFeaturePluginURLs.AddUnique(PluginURL);
		}
		else
		{
			UE_LOG(LogBS, Warning, TEXT("CollectGameFeaturePluginURL Failed"));
		}
	}
}

void UBSCharacterDefManagerComponent::ApplyCharacterDefinition(ABSPlayerState* PlayerState,
                                                               const UBSCharacterDefinition* NewCharacterDef)
{
	if (!NewCharacterDef) return;

	UE_LOG(LogBS, Log, TEXT("Applying new character definition"));

	// GiveAbilities
	GiveAbilities(PlayerState, NewCharacterDef);

	// GameFeatures URL 수집
	CollectGameFeaturePluginURL(NewCharacterDef->GameFeaturesToEnable);
	
	// GameFeatures 활성화
	EnableGameFeatures(PlayerState, GameFeaturePluginURLs, NewCharacterDef);
}

void UBSCharacterDefManagerComponent::EnableGameFeatures(ABSPlayerState* PlayerState,
	const TArray<FString>& GameFeaturesToEnable, const UBSCharacterDefinition* NewCharacterDef) const
{
	UGameFeaturesSubsystem& GameFeatureSubsystem = UGameFeaturesSubsystem::Get();

	for (const FString& FeatureName : GameFeaturesToEnable)
	{
		UE_LOG(LogBS, Log, TEXT("Enabling GameFeature: %s"), *FeatureName);
        
		GameFeatureSubsystem.LoadAndActivateGameFeaturePlugin(
			FeatureName,
			FGameFeaturePluginLoadComplete::CreateLambda([this, FeatureName, NewCharacterDef](const UE::GameFeatures::FResult& Result)
			{
				UE_LOG(LogBS, Warning, TEXT("GameFeature loaded successfully: %s"), *FeatureName);
				OnCharacterDefinitionChangedDelegate.Broadcast(NewCharacterDef);
			})
		);
	}
}

void UBSCharacterDefManagerComponent::DisableGameFeatures(ABSPlayerState* PlayerState,
	const TArray<FString>& GameFeaturesToDisable)
{
	UGameFeaturesSubsystem& GameFeatureSubsystem = UGameFeaturesSubsystem::Get();
	if (IsValid(&GameFeatureSubsystem)) return;

	for (const FString& FeatureName : GameFeaturesToDisable)
	{
		UE_LOG(LogBS, Log, TEXT("Disabling GameFeature: %s"), *FeatureName);
		GameFeatureSubsystem.DeactivateGameFeaturePlugin(FeatureName);
	}
}

void UBSCharacterDefManagerComponent::DisableAllGameFeatures()
{
	for (const FString& FeatureName : GameFeaturePluginURLs)
	{
		UE_LOG(LogBS, Log, TEXT("Disabling GameFeature: %s"), *FeatureName);
		UGameFeaturesSubsystem& GameFeatureSubsystem = UGameFeaturesSubsystem::Get();
		GameFeatureSubsystem.DeactivateGameFeaturePlugin(FeatureName);
	}
}

void UBSCharacterDefManagerComponent::GiveAbilities(ABSPlayerState* PlayerState, const UBSCharacterDefinition* InCharacterDef)
{
	for (const UBSAbilitySet* AbilitySet : InCharacterDef->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(PlayerState->GetBSAbilitySystemComponent(), nullptr);
		}
	}
}

void UBSCharacterDefManagerComponent::OnCharacterDefinitionChanged(const UBSCharacterDefinition* InNewDefinition)
{
	UBSPlayerUISubSystem::Get(this)->OnUICreated.AddLambda([this, InNewDefinition](EUICategory InCategory)
	{
		if (InCategory == EUICategory::Debug)
		{
			UBSPlayerUISubSystem::Get(this)->ShowDebugMessage("DefinitionName",  InNewDefinition->CharacterName);
		}
	});
}
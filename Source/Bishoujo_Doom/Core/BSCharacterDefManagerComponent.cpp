// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/BSCharacterDefManagerComponent.h"

#include "BSCharacterDefinition.h"
#include "BSLogChannels.h"
#include "GameFeatureAction.h"
#include "GameFeatureData.h"
#include "GameFeaturesSubsystem.h"
#include "AbilitySystem/BSAbilitySet.h"
#include "Character/BSPawnData.h"
#include "Engine/AssetManager.h"
#include "GameModes/BSAssetManager.h"
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
	if (IsValid(InPlayerState) && InPlayerState->GetLocalRole() != ROLE_Authority)
	{
		UE_LOG(LogBS, Warning, TEXT("SetCharacterDefinition can only be called on server"));
		return;
	}
	
	const FPrimaryAssetId CharacterDefID("Character", InTag.GetTagLeafName());

	// 1. CharacterDefinition 비동기 로드
	UBSAssetManager::Get().LoadCharacterDefinition(CharacterDefID, FStreamableDelegate::CreateLambda([this, CharacterDefID, InPlayerState, InTag]()
	{
		// 2. CharacterDefinition 비동기 로드 완료
		const auto LoadedCharacterDef = UBSAssetManager::Get().GetPrimaryAssetObject(CharacterDefID);
		if (!LoadedCharacterDef)
		{
			UE_LOG(LogBS, Error, TEXT("Invalid LoadedCharacterDef"));
			return;
		}
		
		const UBSCharacterDefinition* NewCharacterDef = Cast<UBSCharacterDefinition>(LoadedCharacterDef);
		const auto PrevDefData = Cast<ABSPlayerState>(InPlayerState)->GetCharacterDefData();

		if (!IsValid(NewCharacterDef))
		{
			UE_LOG(LogBS, Error, TEXT("Invalid NewCharacterDef"));
			return;
		}

		if (IsValid(PrevDefData) && (PrevDefData->CharacterTag == InTag))
		{
			UE_LOG(LogBS, Error, TEXT("SetCharacterDefinition already applied"));
			return;
		}
		
		// 3. CharacterDefinition 적용
		if (const auto PlayerState = Cast<ABSPlayerState>(InPlayerState))
		{
			if (const UBSCharacterDefinition* OldCharacterDef = PlayerState->GetCharacterDefData())
				CleanupCharacterDefinition(PlayerState, OldCharacterDef);

			ApplyCharacterDefinition(PlayerState, NewCharacterDef);
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
	}));
}

void UBSCharacterDefManagerComponent::CleanupCharacterDefinition(ABSPlayerState* PlayerState,
	const UBSCharacterDefinition* OldCharacterDef)
{
	if (!OldCharacterDef) return;

	// 1. GameFeatures 비활성화
	DisableGameFeatures(PlayerState, OldCharacterDef->GameFeaturesToEnable);

	// 2. CharacterDefinition 언로드
	const FPrimaryAssetId CharacterDefID("Character", OldCharacterDef->CharacterTag.GetTagLeafName());
	UBSAssetManager::Get().UnloadPrimaryAsset(CharacterDefID);

	// 3. Pawn 의 어빌리티 제거
	
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
			UE_LOG(LogBS, Error, TEXT("CollectGameFeaturePluginURL Failed"));
		}
	}
}

void UBSCharacterDefManagerComponent::ApplyCharacterDefinition(ABSPlayerState* PlayerState,
                                                               const UBSCharacterDefinition* NewCharacterDef)
{
	if (!NewCharacterDef) return;

	UE_LOG(LogBS, Log, TEXT("UBSCharacterDefManagerComponent::Applying new character definition"));

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

	for (const FString& PluginURL : GameFeaturesToEnable)
	{
		UE_LOG(LogBS, Log, TEXT("Enabling GameFeature: %s"), *PluginURL);
		
		// GameFeatureSubsystem.GetGameFeatureDataForRegisteredPluginByURL(PluginURL)->GetPluginName(PlayerState->PendingCharacterPluginName);
		// auto State = UE::GameFeatures::ToString(GameFeatureSubsystem.GetPluginState(PluginURL));
		// UE_LOG(LogBS, Log, TEXT("UBSCharacterDefManagerComponent::State1 : %s"), *State);

		GameFeatureSubsystem.LoadGameFeaturePlugin(
			PluginURL,
			FGameFeaturePluginLoadComplete::CreateLambda([this, PlayerState, PluginURL, NewCharacterDef](const UE::GameFeatures::FResult& Result)
			{
				UE_LOG(LogBS, Log, TEXT("GameFeature loaded successfully: %s"), *PluginURL);
				
				UGameFeaturesSubsystem& GameFeatureSubsystem = UGameFeaturesSubsystem::Get();
				GameFeatureSubsystem.GetGameFeatureDataForRegisteredPluginByURL(PluginURL)->GetPluginName(PlayerState->PendingCharacterPluginName);
			})
		);

		// Active만 하는 함수는 없음.
		GameFeatureSubsystem.LoadAndActivateGameFeaturePlugin(
			PluginURL,
			FGameFeaturePluginLoadComplete::CreateLambda([this, PlayerState, PluginURL, NewCharacterDef](const UE::GameFeatures::FResult& Result)
			{
				// 모든 게임 피처 액션들이 Activated 되면 Complete 됨.
				PlayerState->SetCharacterDefData(NewCharacterDef);
				PlayerState->PendingCharacterPluginName.Reset();
				
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
			UE_LOG(LogBS, Log, TEXT("UBSCharacterDefManagerComponent:OnUICreated"));
			UBSPlayerUISubSystem::Get(this)->ShowDebugMessage("DefinitionName",  InNewDefinition->CharacterTag.ToString());
		}
	});
}
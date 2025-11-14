// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/BSCharacterDefSystem.h"

#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "BSCharacterDefinition.h"
#include "BSGameFeatureSystem.h"
#include "Etc/BSLogChannels.h"
#include "GameFeatureData.h"
#include "GameFeaturesSubsystem.h"
#include "AbilitySystem/Abilities/BSAbilitySet.h"
#include "Character/BSPawnData.h"
#include "Engine/AssetManager.h"
#include "GameModes/BSAssetManager.h"
#include "Player/BSPlayerState.h"

UBSCharacterDefSystem::UBSCharacterDefSystem()
	: Super()
{
	OnCharacterDefinitionChangedDelegate.AddDynamic(this, &UBSCharacterDefSystem::OnCharacterDefinitionChanged);

	bIsCharacterDefinitionLoading = false;
}

void UBSCharacterDefSystem::ApplyCharacterDefinition(ABSPlayerState* InPlayerState, const UBSCharacterDefinition* NewCharacterDef)
{
	if (!NewCharacterDef || !InPlayerState) return;

	ApplyPawnData(InPlayerState, NewCharacterDef);
	ApplyGameFeatureAction(NewCharacterDef);
	EnableGameFeatures(InPlayerState, NewCharacterDef->GameFeaturesNameToEnable, NewCharacterDef);
}

void UBSCharacterDefSystem::ApplyPawnData(const ABSPlayerState* InPlayerState,
	const UBSCharacterDefinition* NewCharacterDef)
{
	RespawningPawn(InPlayerState, NewCharacterDef);
}

void UBSCharacterDefSystem::ApplyGameFeatureAction(const UBSCharacterDefinition* NewCharacterDef)
{
	FGameFeatureActivatingContext Context;
	
	// Only apply to our specific world context if set
	const FWorldContext* ExistingWorldContext = GEngine->GetWorldContextFromWorld(GetWorld());
	if (ExistingWorldContext)
	{
		Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);
	}

	if (!NewCharacterDef->DefaultActions.IsEmpty())
	{
		for (UGameFeatureAction* Action : NewCharacterDef->DefaultActions)
		{
			if (Action != nullptr)
			{
				Action->OnGameFeatureRegistering();
				Action->OnGameFeatureLoading();
				Action->OnGameFeatureActivating(Context);
			}
		}
	}
}

bool UBSCharacterDefSystem::RespawningPawn(const ABSPlayerState* InPlayerState,
                                           const UBSCharacterDefinition* NewCharacterDef)
{
	APawn* CurrentPawn = InPlayerState->GetPawn();
	if (!IsValid(CurrentPawn))
	{
		UE_LOG(LogBS, Error, TEXT("UBSCharacterDefManagerComponent::GetPawn is nullptr"));
		return false;
	}

	const UBSPawnData* NewPawnData = NewCharacterDef->PawnData;
	if (!IsValid(NewPawnData->PawnClass))
	{
		UE_LOG(LogBS, Error, TEXT("UBSCharacterDefManagerComponent::NewPawnData->PawnClass is nullptr"));
		return false;
	}

	if (NewPawnData->PawnClass == CurrentPawn->GetClass())
	{
		UE_LOG(LogBS, Log, TEXT("UBSCharacterDefManagerComponent::NewPawnData->PawnClass and CurrentPawnClass is Same"));
		return false;
	}

	AController* Controller = InPlayerState->GetOwner<AController>();
	check(Controller);
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Controller;
	SpawnParams.Instigator = CurrentPawn->GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	FVector SpawnLocation = CurrentPawn->GetActorLocation();
	FRotator SpawnRotation = CurrentPawn->GetActorRotation();
	
	CurrentPawn->Destroy();

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SpawnLocation);
	SpawnTransform.SetRotation(SpawnRotation.Quaternion());
	
	APawn* NewPawn = GetWorld()->SpawnActorDeferred<APawn>(NewPawnData->PawnClass, SpawnTransform);
	// Pawn 과 Component 들의 BeginPlay 에서 PlayerState 를 찾을 수 없음.
	// Possess 안되었기 때문임.
	// 따라서 Deffered로 Component 들의 BeginPlay 를 나중에 호출하게 함.
	
	if (NewPawn == nullptr)
	{
		UE_LOG(LogBS, Error, TEXT("UBSCharacterDefManagerComponent::Spawn NewPawn is failed"));
		return false;
	}
	
	Controller->Possess(NewPawn);

	if (!IsValid(NewPawn->GetPlayerState()))
	{
		UE_LOG(LogBS, Error, TEXT("UBSCharacterDefManagerComponent::NewPawn GetPlayerState is failed"));
		return false;
	}

	NewPawn->FinishSpawning(SpawnTransform);

	UE_LOG(LogBS, Log, TEXT("UBSCharacterDefManagerComponent::ApplyPawnData: Successfully spawned and possessed new Pawn: %s"), *NewPawn->GetName());

	return true;
}

void UBSCharacterDefSystem::SetCharacterDefinition(APlayerState* InPlayerState,
                                                   const FGameplayTag InTag)
{
	if (IsValid(InPlayerState) && InPlayerState->GetLocalRole() != ROLE_Authority)
	{
		UE_LOG(LogBS, Warning, TEXT("SetCharacterDefinition can only be called on server"));
		return;
	}

	const auto PrevDefData = Cast<ABSPlayerState>(InPlayerState)->GetCharacterDefData();
	if (IsValid(PrevDefData) && (PrevDefData->CharacterTag == InTag))
	{
		return;
	}
	
	if (bIsCharacterDefinitionLoading)
	{
		PendingCharacterDefinitionArray.Add({InPlayerState, InTag});
		return;
	}

	bIsCharacterDefinitionLoading = true;
	
	const FPrimaryAssetId CharacterDefID("Character", InTag.GetTagLeafName());

	// 1. CharacterDefinition 비동기 로드
	UBSAssetManager::Get().LoadCharacterDefinition(CharacterDefID, FStreamableDelegate::CreateLambda([this, CharacterDefID, InPlayerState]()
	{
		// 2. CharacterDefinition 비동기 로드 완료
		const auto LoadedCharacterDef = UBSAssetManager::Get().GetPrimaryAssetObject(CharacterDefID);
		if (!LoadedCharacterDef)
		{
			UE_LOG(LogBS, Error, TEXT("Invalid LoadedCharacterDef"));
			return;
		}
		
		const UBSCharacterDefinition* NewCharacterDef = Cast<UBSCharacterDefinition>(LoadedCharacterDef);

		if (!IsValid(NewCharacterDef))
		{
			UE_LOG(LogBS, Error, TEXT("Invalid NewCharacterDef"));
			return;
		}
		
		// 3. CharacterDefinition 적용
		if (const auto PlayerState = Cast<ABSPlayerState>(InPlayerState))
		{
			if (const UBSCharacterDefinition* OldCharacterDef = PlayerState->GetCharacterDefData())
				CleanupCharacterDefinition(PlayerState, OldCharacterDef);

			ApplyCharacterDefinition(PlayerState, NewCharacterDef);
		}
	}));
}

void UBSCharacterDefSystem::K2_SetCharacterDefinition(APlayerState* InPlayerState, FGameplayTag InTag)
{
	SetCharacterDefinition(InPlayerState, InTag);
}

void UBSCharacterDefSystem::BeginDestroy()
{
	Super::BeginDestroy();
}

void UBSCharacterDefSystem::OnActionDeactivationCompleted()
{
	UE_LOG(LogBS, Warning, TEXT("UBSCharacterDefSystem::OnActionDeactivationCompleted"));
}

void UBSCharacterDefSystem::CleanupCharacterDefinition(ABSPlayerState* InPlayerState,
                                                       const UBSCharacterDefinition* OldCharacterDef)
{
	if (!OldCharacterDef) return;

	// 0. DefaultInputMappingContext 제거 (DefaultInputSet 의 InputAction 은 UInputComponent가 제거되면서 자동 제거됨)
	const APlayerController* PC = InPlayerState->GetPlayerController();
	check(PC);

	const ULocalPlayer* LP = Cast<ULocalPlayer>(PC->GetLocalPlayer());
	check(LP);

	// 1. GameFeatures 비활성화
	DisableGameFeatures(OldCharacterDef->GameFeaturesNameToEnable);

	// 2. DefaultGameFeatureActions 비활성화
	DisableGameFeatureActions(OldCharacterDef);

	// 2. CharacterDefinition 언로드
	const FPrimaryAssetId CharacterDefID("Character", OldCharacterDef->CharacterTag.GetTagLeafName());
	UBSAssetManager::Get().UnloadPrimaryAsset(CharacterDefID);
}

void UBSCharacterDefSystem::DisableGameFeatureActions(const UBSCharacterDefinition* OldCharacterDef)
{
	// Deactivate and unload the actions
	FGameFeatureDeactivatingContext Context(TEXT(""), [this](FStringView) { this->OnActionDeactivationCompleted(); });
	const FWorldContext* ExistingWorldContext = GEngine->GetWorldContextFromWorld(GetWorld());
	if (ExistingWorldContext)
	{
		Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);
	}

	if (!OldCharacterDef->DefaultActions.IsEmpty())
	{
		for (UGameFeatureAction* Action : OldCharacterDef->DefaultActions)
		{
			if (Action != nullptr)
			{
				Action->OnGameFeatureDeactivating(Context);
				Action->OnGameFeatureUnregistering();
			}
		}
	}
}

void UBSCharacterDefSystem::EnableGameFeatures(ABSPlayerState* InPlayerState,
                                               const TArray<FString>& InGameFeaturesNameToEnable, const UBSCharacterDefinition* NewCharacterDef)
{
	if (const auto BSGameFeatureSystem = GetWorld()->GetGameInstance<UGameInstance>()->GetSubsystem<UBSGameFeatureSystem>())
	{
		RequiredEnableCount = InGameFeaturesNameToEnable.Num();
		
		for (const FString& FeatureName : InGameFeaturesNameToEnable)
		{
			FString PluginURL = BSGameFeatureSystem->GetPluginURLByName(FeatureName);
			BSGameFeatureSystem->EnableGameFeature(
				FeatureName,
				FGameFeaturePluginLoadComplete::CreateLambda([this, InPlayerState, PluginURL](const UE::GameFeatures::FResult& Result)
				{
					UE_LOG(LogBS, Log, TEXT("GameFeature loaded successfully: %s"), *PluginURL);
				}),

				FGameFeaturePluginLoadComplete::CreateLambda([this, InPlayerState, FeatureName, NewCharacterDef](const UE::GameFeatures::FResult& Result)
				{
					// 모든 게임 피처 액션들이 Activated 되면 Complete 됨.
					UE_LOG(LogBS, Log, TEXT("GameFeature Active successfully: %s"), *FeatureName);

					RequiredEnableCount--;

					if (RequiredEnableCount == 0)
					{
						InPlayerState->SetCharacterDefData(NewCharacterDef);
						OnCharacterDefinitionChangedDelegate.Broadcast(InPlayerState, NewCharacterDef);
					}
				}));
		}
	}

	if (InGameFeaturesNameToEnable.IsEmpty())
	{
		InPlayerState->SetCharacterDefData(NewCharacterDef);
		OnCharacterDefinitionChangedDelegate.Broadcast(InPlayerState, NewCharacterDef);
	}
}

void UBSCharacterDefSystem::DisableGameFeatures(const TArray<FString>& InGameFeaturesToDisable)
{
	const auto BSGameFeatureSystem = GetWorld()->GetGameInstance<UGameInstance>()->GetSubsystem<UBSGameFeatureSystem>();
	BSGameFeatureSystem->DisableGameFeatures(InGameFeaturesToDisable);
}

void UBSCharacterDefSystem::OnCharacterDefinitionChanged(const ABSPlayerState* InBSPlayerState, const UBSCharacterDefinition* InNewDefinition)
{
	bIsCharacterDefinitionLoading = false;
	
	UBSPlayerUISubSystem::Get(this)->ShowPawnAbilitySetMessage(InBSPlayerState, InNewDefinition);
	if (!PendingCharacterDefinitionArray.IsEmpty())
	{
		auto Element = PendingCharacterDefinitionArray.Pop();
		SetCharacterDefinition(Element.PlayerState, Element.Tag);
	}
}

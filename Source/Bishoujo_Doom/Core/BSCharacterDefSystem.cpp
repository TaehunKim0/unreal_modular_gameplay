// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/BSCharacterDefSystem.h"

#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "BSCharacterDefinition.h"
#include "BSGameFeatureSystem.h"
#include "BSLogChannels.h"
#include "EnhancedInputSubsystems.h"
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
}

void UBSCharacterDefSystem::ApplyCharacterDefinition(ABSPlayerState* InPlayerState,const UBSCharacterDefinition* NewCharacterDef)
{
	if (!NewCharacterDef) return;

	ApplyPawnData(InPlayerState, NewCharacterDef);
	GiveAbilitySet(InPlayerState, NewCharacterDef);
	EnableGameFeatures(InPlayerState, NewCharacterDef->GameFeaturesNameToEnable, NewCharacterDef);
}

void UBSCharacterDefSystem::ApplyPawnData(const ABSPlayerState* InPlayerState,
	const UBSCharacterDefinition* NewCharacterDef)
{
	RespawningPawn(InPlayerState, NewCharacterDef);
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

	APawn* NewPawn = GetWorld()->SpawnActor<APawn>(NewPawnData->PawnClass, SpawnLocation, SpawnRotation, SpawnParams);
	// Pawn 과 Component 들의 BeginPlay 에서 PlayerState 를 찾을 수 없음.
	// Possess 안되었기 때문임.
	
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

	UE_LOG(LogBS, Log, TEXT("UBSCharacterDefManagerComponent::ApplyPawnData: Successfully spawned and possessed new Pawn: %s"), *NewPawn->GetName());

	return true;
}

void UBSCharacterDefSystem::GiveAbilitySet(const ABSPlayerState* InPlayerState,	const UBSCharacterDefinition* NewCharacterDef)
{
	UAbilitySystemComponent* ASC = InPlayerState->GetAbilitySystemComponent();
	ensure(ASC);

	// GameplayAbility
	if (!NewCharacterDef->DefaultAbilitySet)
	{
		UE_LOG(LogBS, Warning, TEXT("UBSCharacterDefSystem::DefaultAbilitySet is nullptr"));
		return;
	}
		
	for (const FBSAbilitySet_GameplayAbility& BSGrantAbility : NewCharacterDef->DefaultAbilitySet->GrantAbilitiesWithInputTag)
	{
		FGameplayAbilitySpec Spec(BSGrantAbility.Ability, BSGrantAbility.AbilityLevel, INDEX_NONE, InPlayerState->GetOwner());
		Spec.GetDynamicSpecSourceTags().AddTag(BSGrantAbility.InputTag);
				
		FGameplayAbilitySpecHandle AbilityHandle = ASC->GiveAbility(Spec);
	}

	// Attribute
	for (const FBSAbilitySet_AttributeSet BSAttributeSet : InPlayerState->GetCharacterDefData()->DefaultAbilitySet->GrantAttributeSets)
	{
		if (BSAttributeSet.AttributeSet)
		{
			UAttributeSet* NewSet = NewObject<UAttributeSet>(ASC->GetOwner(), BSAttributeSet.AttributeSet);
			ASC->AddAttributeSetSubobject(NewSet);
		}
	}

	// GameplayEffect
	for (const FBSAbilitySet_GameplayEffect EffectToGrant : InPlayerState->GetCharacterDefData()->DefaultAbilitySet->GrantGameplayEffects)
	{
		if (EffectToGrant.GameplayEffect)
		{
			const UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
			const FActiveGameplayEffectHandle GameplayEffectHandle = ASC->ApplyGameplayEffectToSelf(GameplayEffect, EffectToGrant.EffectLevel, ASC->MakeEffectContext());
		}
	}
}

void UBSCharacterDefSystem::SetCharacterDefinition(APlayerState* InPlayerState,
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
	}));
}

void UBSCharacterDefSystem::BeginDestroy()
{
	Super::BeginDestroy();
}

void UBSCharacterDefSystem::CleanupCharacterDefinition(ABSPlayerState* PlayerState,
                                                                 const UBSCharacterDefinition* OldCharacterDef)
{
	if (!OldCharacterDef) return;

	// 0. DefaultInputMappingContext 제거 (DefaultInputSet 의 InputAction 은 UInputComponent가 제거되면서 자동 제거됨)
	const APlayerController* PC = PlayerState->GetPlayerController();
	check(PC);

	const ULocalPlayer* LP = Cast<ULocalPlayer>(PC->GetLocalPlayer());
	check(LP);

	// 1. GameFeatures 비활성화
	DisableGameFeatures(OldCharacterDef->GameFeaturesNameToEnable);

	// 2. CharacterDefinition 언로드
	const FPrimaryAssetId CharacterDefID("Character", OldCharacterDef->CharacterTag.GetTagLeafName());
	UBSAssetManager::Get().UnloadPrimaryAsset(CharacterDefID);
}

void UBSCharacterDefSystem::EnableGameFeatures(ABSPlayerState* InPlayerState,
	const TArray<FString>& GameFeaturesNameToEnable, const UBSCharacterDefinition* NewCharacterDef)
{
	if (const auto BSGameFeatureSystem = GetWorld()->GetGameInstance<UGameInstance>()->GetSubsystem<UBSGameFeatureSystem>())
	{
		for (const FString& FeatureName : GameFeaturesNameToEnable)
		{
			FString PluginURL = BSGameFeatureSystem->GetPluginURLByName(FeatureName);
			BSGameFeatureSystem->EnableGameFeature(
				FeatureName,
				FGameFeaturePluginLoadComplete::CreateLambda([this, InPlayerState, PluginURL](const UE::GameFeatures::FResult& Result)
				{
					UE_LOG(LogBS, Log, TEXT("GameFeature loaded successfully: %s"), *PluginURL);
					
					UGameFeaturesSubsystem& GameFeatureSubsystem = UGameFeaturesSubsystem::Get();
					GameFeatureSubsystem.GetGameFeatureDataForRegisteredPluginByURL(PluginURL)->GetPluginName(InPlayerState->PendingCharacterPluginName);
				}),

				FGameFeaturePluginLoadComplete::CreateLambda([this, InPlayerState, FeatureName, NewCharacterDef](const UE::GameFeatures::FResult& Result)
				{
					// 모든 게임 피처 액션들이 Activated 되면 Complete 됨.
					UE_LOG(LogBS, Log, TEXT("GameFeature Active successfully: %s"), *FeatureName);
					
					InPlayerState->SetCharacterDefData(NewCharacterDef);
					InPlayerState->PendingCharacterPluginName.Reset();

					OnCharacterDefinitionChangedDelegate.Broadcast(InPlayerState, NewCharacterDef);
				}));
		}
	}

	if (GameFeaturesNameToEnable.IsEmpty())
	{
		InPlayerState->SetCharacterDefData(NewCharacterDef);
		InPlayerState->PendingCharacterPluginName.Reset();
				
		OnCharacterDefinitionChangedDelegate.Broadcast(InPlayerState, NewCharacterDef);
	}
}

void UBSCharacterDefSystem::DisableGameFeatures(const TArray<FString>& GameFeaturesToDisable)
{
	const auto BSGameFeatureSystem = GetWorld()->GetGameInstance<UGameInstance>()->GetSubsystem<UBSGameFeatureSystem>();
	BSGameFeatureSystem->DisableGameFeatures(GameFeaturesToDisable);
}

void UBSCharacterDefSystem::OnCharacterDefinitionChanged(const ABSPlayerState* InBSPlayerState, const UBSCharacterDefinition* InNewDefinition)
{
	UBSPlayerUISubSystem::Get(this)->ShowPawnAbilitySetMessage(InBSPlayerState, InNewDefinition);
}

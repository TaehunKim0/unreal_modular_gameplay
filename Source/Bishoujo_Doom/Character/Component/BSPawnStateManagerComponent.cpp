// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/BSPawnStateManagerComponent.h"

#include "Etc/BSGamePlayTags.h"
#include "BSHealthComponent.h"
#include "BSPawnInputComponent.h"
#include "AbilitySystem/BSAbilitySystemComponent.h"
#include "Character/BSCharacter.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameModes/BSGameState.h"
#include "Input/BSInputComponent.h"
#include "Player/BSPlayerState.h"

const FName UBSPawnStateManagerComponent::NAME_PAWNSTATEMANAGERCOMPONENT("PawnStateManagerComponent");

UBSPawnStateManagerComponent::UBSPawnStateManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UBSPawnStateManagerComponent::OnRegister()
{
	Super::OnRegister();
	RegisterInitStateFeature();
}

void UBSPawnStateManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);

	const auto CharacterDefSystem = GetGameInstance<UGameInstance>()->GetSubsystem<UBSCharacterDefSystem>();
	CharacterDefSystem->OnCharacterDefinitionChangedDelegate.AddDynamic(this, &UBSPawnStateManagerComponent::OnCharacterDefinitionChanged);
	
	TryToChangeInitState(BSGamePlayTags::InitState_Spawned);
	CheckDefaultInitialization();
}

void UBSPawnStateManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();
	
	Super::EndPlay(EndPlayReason);
}

void UBSPawnStateManagerComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UBSPawnStateManagerComponent::OnCharacterDefinitionChanged(const ABSPlayerState* InPlayerState, const UBSCharacterDefinition* InNewDefinition)
{
	CheckDefaultInitialization();
}

//~ Begin IGameFrameworkInitStateInterface interface
bool UBSPawnStateManagerComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
                                                   FGameplayTag DesiredState) const
{
	if (DesiredState == BSGamePlayTags::InitState_Spawned)
	{
		return true;
	}

	if (DesiredState == BSGamePlayTags::InitState_DataAvailable)
	{
		if (!Manager->HaveAllFeaturesReachedInitState(GetOwningActor(), BSGamePlayTags::InitState_Spawned))
		{
			return false;
		}

		UE_LOG(LogBSInitState, Warning, TEXT("UBSPawnStateManagerComponent::InitState_Spawned"));
		return true;
	}

	if (DesiredState == BSGamePlayTags::InitState_DataInitialized)
	{
		if (!Manager->HaveAllFeaturesReachedInitState(GetOwningActor(), BSGamePlayTags::InitState_DataAvailable))
		{
			return false;
		}

		UE_LOG(LogBSInitState, Warning, TEXT("UBSPawnStateManagerComponent::InitState_DataAvailable"));
		return true;
	}

	if (DesiredState == BSGamePlayTags::InitState_GameplayReady)
	{
		if (!Manager->HaveAllFeaturesReachedInitState(GetOwningActor(), BSGamePlayTags::InitState_DataInitialized))
		{
			return false;
		}

		UE_LOG(LogBSInitState, Warning, TEXT("UBSPawnStateManagerComponent::InitState_DataInitialized"));
		return true;
	}

	if (CurrentState == BSGamePlayTags::InitState_GameplayReady)
	{
		return true;
	}

	return false;
}

void UBSPawnStateManagerComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager,
	FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	UE_LOG(LogBSInitState, Log, TEXT("UBSPawnStateManagerComponent::	Actor State %s -> %s"), *CurrentState.ToString(), *DesiredState.ToString());

	if (DesiredState == BSGamePlayTags::InitState_GameplayReady)
	{
		UE_LOG(LogBSInitState, Warning, TEXT("UBSPawnStateManagerComponent::InitState_GameplayReady"));
		OnPawnGameplayReadyCompleted.Broadcast();
	}
}

void UBSPawnStateManagerComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName != NAME_PAWNSTATEMANAGERCOMPONENT)
	{
		CheckDefaultInitialization();
	}
}

void UBSPawnStateManagerComponent::CheckDefaultInitialization()
{
	static const TArray<FGameplayTag> StateChain = { BSGamePlayTags::InitState_Spawned,BSGamePlayTags::InitState_DataAvailable, BSGamePlayTags::InitState_DataInitialized,BSGamePlayTags::InitState_GameplayReady };
	ContinueInitStateChain(StateChain);
}
//~ End IGameFrameworkInitStateInterface interface
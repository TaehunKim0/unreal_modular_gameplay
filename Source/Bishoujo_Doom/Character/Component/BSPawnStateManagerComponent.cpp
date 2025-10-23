// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/BSPawnStateManagerComponent.h"

#include "BSGamePlayTags.h"
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

	ABSCharacter* BSCharacter = GetPawn<ABSCharacter>();
	BSCharacter->ReceiveRestartedDelegate.AddDynamic(this, &UBSPawnStateManagerComponent::OnPawnRestarted);
	BSCharacter->OnRepPlayerStateDelegate.AddUObject(this, &UBSPawnStateManagerComponent::OnPlayerStateChanged);
	
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

void UBSPawnStateManagerComponent::OnPawnRestarted(APawn* NewPawn)
{
	CheckDefaultInitialization();
}

void UBSPawnStateManagerComponent::OnPlayerStateChanged(APlayerState* NewPlayerState)
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

	if (DesiredState == BSGamePlayTags::InitState_PlayerStateInitialized)
	{
		if (const auto Pawn = GetPawn<APawn>(); !Pawn->GetPlayerState())
			return false;

		if (const auto Pawn = GetPawn<APawn>(); !Pawn->InputComponent)
			return false;
		
		if (!Manager->HaveAllFeaturesReachedInitState(GetOwningActor(), BSGamePlayTags::InitState_Spawned))
		{
			return false;
		}

		UE_LOG(LogBSInitState, Warning, TEXT("UBSPawnStateManagerComponent::InitState_PlayerStateInitialized"));
		return true;
	}

	if (DesiredState == BSGamePlayTags::InitState_ASCInitialized)
	{
		const auto BSPlayerState = GetPlayerState<ABSPlayerState>();
		ensure(BSPlayerState);
		
		if (!Manager->HaveAllFeaturesReachedInitState(GetOwningActor(), BSGamePlayTags::InitState_PlayerStateInitialized))
		{
			return false;
		}
		
		if (BSPlayerState && BSPlayerState->GetBSAbilitySystemComponent()->HasBeenInitialized())
		{
			UE_LOG(LogBSInitState, Warning, TEXT("UBSPawnStateManagerComponent::InitState_ASCInitialized"));
			return true;
		}
		
		return false;
	}

	if (DesiredState == BSGamePlayTags::InitState_CharacterDefinitionInitialized)
	{
		const auto BSPlayerState = GetPlayerState<ABSPlayerState>();
		ensure(BSPlayerState);
		
		if (!Manager->HaveAllFeaturesReachedInitState(GetOwningActor(), BSGamePlayTags::InitState_ASCInitialized, NAME_PAWNSTATEMANAGERCOMPONENT))
		{
			return false;
		}
		
		if (BSPlayerState && BSPlayerState->GetCharacterDefData())
		{
			UE_LOG(LogBSInitState, Warning, TEXT("UBSPawnStateManagerComponent::InitState_CharacterDefinitionInitialized"));
			return true;
		}

		UE_LOG(LogBSInitState, Error, TEXT("UBSPawnStateManagerComponent:: CharacterDefinition is not exist!"));
		return false;
	}

	if (DesiredState == BSGamePlayTags::InitState_CharacterComponentInitialized)
	{
		if (!Manager->HaveAllFeaturesReachedInitState(GetOwningActor(), BSGamePlayTags::InitState_CharacterDefinitionInitialized, NAME_PAWNSTATEMANAGERCOMPONENT))
		{
			return false;
		}

		UE_LOG(LogBSInitState, Warning, TEXT("UBSPawnStateManagerComponent::InitState_CharacterComponentInitialized"));
		return true;
	}

	if (DesiredState == BSGamePlayTags::InitState_GameplayReady)
	{
		if (!Manager->HaveAllFeaturesReachedInitState(GetOwningActor(), BSGamePlayTags::InitState_CharacterComponentInitialized, NAME_PAWNSTATEMANAGERCOMPONENT))
		{
			return false;
		}

		UE_LOG(LogBSInitState, Warning, TEXT("UBSPawnStateManagerComponent::InitState_GameplayReady"));
		return true;
	}

	return false;
}

void UBSPawnStateManagerComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager,
	FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	UE_LOG(LogBSInitState, Log, TEXT("UBSPawnStateManagerComponent::Actor State %s -> %s"), *CurrentState.ToString(), *DesiredState.ToString());

	if (CurrentState == BSGamePlayTags::InitState_ASCInitialized)
	{
		const auto BSPlayerState = GetPlayerState<ABSPlayerState>();
		ensure(BSPlayerState);
		
		if (BSPlayerState && BSPlayerState->GetBSAbilitySystemComponent())
		{
			OnAbilitySystemInitialized.Broadcast();
		}
	}

	if (DesiredState == BSGamePlayTags::InitState_GameplayReady)
	{
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
	static const TArray<FGameplayTag> StateChain = { BSGamePlayTags::InitState_Spawned,BSGamePlayTags::InitState_PlayerStateInitialized, BSGamePlayTags::InitState_ASCInitialized, BSGamePlayTags::InitState_CharacterDefinitionInitialized, BSGamePlayTags::InitState_CharacterComponentInitialized, BSGamePlayTags::InitState_GameplayReady };
	ContinueInitStateChain(StateChain);
}
//~ End IGameFrameworkInitStateInterface interface
// Fill out your copyright notice in the Description page of Project Settings.


#include "BSPlayerState.h"

#include "BSLogChannels.h"
#include "AbilitySystem/BSAbilitySystemComponent.h"
#include "BSPlayerController.h"
#include "AbilitySystem/BSAbilitySet.h"
#include "Character/BSPawnData.h"
#include "Core/BSCharacterDefinition.h"
#include "GameModes/BSGameState.h"
#include "UI/SubSystem/BSPlayerUISubSystem.h"

ABSPlayerState::ABSPlayerState(const FObjectInitializer& ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UBSAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// AbilitySystemComponent needs to be updated at a high frequency.
	SetNetUpdateFrequency(100.0f);

	UE_LOG(LogBS, Log, TEXT("ABSPlayerState::ABSPlayerState"));
}

void ABSPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	// UE_LOG(LogBS, Log, TEXT("ABSPlayerState::PostInitializeComponents"));
	//
	// check(AbilitySystemComponent);
	// AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());
	//
	// ABSGameState* BSGameState = GetWorld()->GetGameState<ABSGameState>();
	// if (!BSGameState)
	// {
	// 	UE_LOG(LogBS, Log, TEXT("BSGameState is null in SetupDefaultCharacterForPlayer"));
	// 	return;
	// }
	//
	// SetCharacterDefData(BSGameState->GetDefaultCharacterDefinition());
}

ABSPlayerController* ABSPlayerState::GetBSPlayerController() const
{
	return Cast<ABSPlayerController>(GetOwner());
}

void ABSPlayerState::GetLifetimeReplicatedProps(
	TArray<class FLifetimeProperty, TSizedDefaultAllocator<32>>& LifetimeProperties) const
{
	Super::GetLifetimeReplicatedProps(LifetimeProperties);
}

UAbilitySystemComponent* ABSPlayerState::GetAbilitySystemComponent() const
{
	return GetBSAbilitySystemComponent();
}

// TODO : CharacterDefManagerComponent 랑 겹친다.
void ABSPlayerState::SetCharacterDefData(const UBSCharacterDefinition* InCharacterDefData)
{
	check(InCharacterDefData);

	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	CharacterDefData = InCharacterDefData;
	
	UE_LOG(LogBS, Warning, TEXT("ABSPlayerState::SetCharacterDefData"));
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "BSGameState.h"

#include "BSAssetManager.h"
#include "Core/BSCharacterDefinition.h"
#include "BSLogChannels.h"
#include "Net/UnrealNetwork.h"

ABSGameState::ABSGameState()
{
	UE_LOG(LogBS, Log, TEXT("ABSGameStateBase::ABSGameStateBase"));

	CharacterDefManagerComponent = CreateDefaultSubobject<UBSCharacterDefManagerComponent>(TEXT("CharacterDefManagerComponent"));
}

void ABSGameState::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		return;
	}
}

void ABSGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ABSGameState::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	UE_LOG(LogBS, Log, TEXT("ABSGameStateBase::PreInitializeComponents"));
}

void ABSGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	UE_LOG(LogBS, Log, TEXT("ABSGameStateBase::PostInitializeComponents"));
}

void ABSGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "BSGameStateBase.h"

#include "BSAssetManager.h"
#include "Character/BSCharacterDefinition.h"
#include "BSLogChannels.h"
#include "Net/UnrealNetwork.h"

ABSGameStateBase::ABSGameStateBase()
{
	UE_LOG(LogBS, Log, TEXT("ABSGameStateBase::ABSGameStateBase"));
}

void ABSGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		return;
	}
}

void ABSGameStateBase::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	UE_LOG(LogBS, Log, TEXT("ABSGameStateBase::PreInitializeComponents"));
}

void ABSGameStateBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	UE_LOG(LogBS, Log, TEXT("ABSGameStateBase::PostInitializeComponents"));
}

void ABSGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
	DOREPLIFETIME(ABSGameStateBase, DefaultCharacterDefinition);
}

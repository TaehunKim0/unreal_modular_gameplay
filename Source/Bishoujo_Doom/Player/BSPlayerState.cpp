// Fill out your copyright notice in the Description page of Project Settings.


#include "BSPlayerState.h"

#include "BSLogChannels.h"
#include "AbilitySystem/BSAbilitySystemComponent.h"
#include "BSPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "AbilitySystem/Abilities/BSAbilitySet.h"
#include "AbilitySystem/Attributes/BSHealthAttributeSet.h"
#include "Character/BSPawnData.h"
#include "Core/BSCharacterDefinition.h"
#include "GameModes/BSAssetManager.h"
#include "GameModes/BSGameState.h"
#include "UI/SubSystem/BSPlayerUISubSystem.h"

ABSPlayerState::ABSPlayerState(const FObjectInitializer& ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UBSAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	CharacterDefData = nullptr;

	HealthAttributeSet = CreateDefaultSubobject<UBSHealthAttributeSet>(TEXT("HealthAttributeSet"));

	// AbilitySystemComponent needs to be updated at a high frequency.
	SetNetUpdateFrequency(100.0f);

	PrimaryActorTick.bCanEverTick = true;
	
	UE_LOG(LogBS, Log, TEXT("ABSPlayerState::ABSPlayerState"));
}

void ABSPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());

	const FPrimaryAssetId DefaultCharacterDefID("Character", "Default");
	auto NewCharacterDef = UBSAssetManager::Get().LoadCharacterDefinitionSynchronously(DefaultCharacterDefID);
	CharacterDefData = NewCharacterDef;

	
}

void ABSPlayerState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
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
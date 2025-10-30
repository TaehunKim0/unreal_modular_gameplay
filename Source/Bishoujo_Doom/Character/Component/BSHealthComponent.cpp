// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/BSHealthComponent.h"

#include "BSGamePlayTags.h"
#include "BSLogChannels.h"
#include "BSPawnInputComponent.h"
#include "BSPawnStateManagerComponent.h"
#include "AbilitySystem/BSAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/BSHealthAttributeSet.h"
#include "Character/BSCharacter.h"
#include "Components/GameFrameworkComponentDelegates.h"
#include "Components/GameFrameworkComponentManager.h"
#include "UI/SubSystem/BSPlayerUISubSystem.h"

const FName UBSHealthComponent::NAME_HEALTHCOMPONENT("HealthComponent");

UBSHealthComponent::UBSHealthComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UBSHealthComponent::OnRegister()
{
	Super::OnRegister();
	
	RegisterInitStateFeature();
}

void UBSHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// Name_None = Listen for changes to all features
	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);

	ensure(TryToChangeInitState(BSGamePlayTags::InitState_Spawned));
	CheckDefaultInitialization();
}

void UBSHealthComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UnregisterInitStateFeature();
}

void UBSHealthComponent::InitializeWithAbilitySystem(UBSAbilitySystemComponent* InAbilitySystem)
{
	AActor* Owner = GetOwner();
	check(Owner);

	if (AbilitySystemComponent)
	{
		return;
	}

	AbilitySystemComponent = InAbilitySystem;
	if (!AbilitySystemComponent)
	{
		return;
	}

	HealthSet = AbilitySystemComponent->GetSet<UBSHealthAttributeSet>();
	if (!HealthSet)
	{
		return;
	}

	HealthSet->OnHealthChanged.AddUObject(this, &ThisClass::HandleHealthChanged);
	HealthSet->OnMaxHealthChanged.AddUObject(this, &ThisClass::HandleMaxHealthChanged);
	//HealthSet->OnOutOfHealth.AddUObject(this, &ThisClass::HandleOutOfHealth);
	
	AbilitySystemComponent->SetNumericAttributeBase(UBSHealthAttributeSet::GetHealthAttribute(), HealthSet->GetMaxHealth());
	//ClearGameplayTags();

	OnHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
	OnMaxHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
}

void UBSHealthComponent::HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser,
                                             const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{

	OnHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
}

void UBSHealthComponent::HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser,
	const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
	OnMaxHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
}


//~ Begin IGameFrameworkInitStateInterface interface
//
//
void UBSHealthComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
											   FGameplayTag DesiredState)
{
	UE_LOG(LogBSInitState, Log, TEXT("UBSHealthComponent::Actor State %s -> %s"), *CurrentState.ToString(), *DesiredState.ToString());
	
	if (CurrentState == BSGamePlayTags::InitState_ASCInitialized)
	{
		const auto BSCharacter = Cast<ABSCharacter>(GetOwningActor());
		if (BSCharacter)
		{
			InitializeWithAbilitySystem(BSCharacter->GetBSAbilitySystemComponent());
		}
	}
}

bool UBSHealthComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	FGameplayTag DesiredState) const
{
	if (DesiredState == BSGamePlayTags::InitState_ASCInitialized)
	{
		return Manager->HasFeatureReachedInitState(GetOwningActor(), UBSPawnStateManagerComponent::NAME_PAWNSTATEMANAGERCOMPONENT,BSGamePlayTags::InitState_ASCInitialized);
	}

	return true;
}

void UBSHealthComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName != NAME_HEALTHCOMPONENT)
	{
		CheckDefaultInitialization();
	}
}

void UBSHealthComponent::CheckDefaultInitialization()
{
	static const TArray<FGameplayTag> StateChain = { BSGamePlayTags::InitState_Spawned,BSGamePlayTags::InitState_PlayerStateInitialized, BSGamePlayTags::InitState_InputComponentInitialized, BSGamePlayTags::InitState_ASCInitialized, BSGamePlayTags::InitState_CharacterDefinitionInitialized, BSGamePlayTags::InitState_CharacterComponentInitialized, BSGamePlayTags::InitState_GameplayReady };
	ContinueInitStateChain(StateChain);
}

float UBSHealthComponent::GetHealth()
{
	return HealthSet->GetHealth();
}

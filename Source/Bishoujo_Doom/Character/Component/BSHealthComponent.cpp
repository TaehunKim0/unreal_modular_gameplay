// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/BSHealthComponent.h"

#include "Etc/BSGamePlayTags.h"
#include "Etc/BSLogChannels.h"
#include "BSPawnInputComponent.h"
#include "BSPawnStateManagerComponent.h"
#include "AbilitySystem/BSAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/BSHealthAttributeSet.h"
#include "Character/BSCharacter.h"
#include "Components/GameFrameworkComponentDelegates.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Player/BSPlayerState.h"
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
	if (DesiredState == BSGamePlayTags::InitState_DataInitialized)
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
	if (DesiredState == BSGamePlayTags::InitState_DataInitialized)
	{
		const auto PS = Cast<APawn>(GetOwningActor())->GetPlayerState();
		if (!PS)
			return false;

		// 이 부분
		if (Manager->HasFeatureReachedInitState(
			PS, ABSPlayerState::NAME_PLAYERSTATE,
			BSGamePlayTags::InitState_DataInitialized)
			
		&& Manager->HasFeatureReachedInitState(
			PS, UBSAbilitySystemComponent::NAME_ABILITYSYSTEMCOMPONENT,
			BSGamePlayTags::InitState_DataInitialized))
		{
			return true;
		}

		return false;
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
	static const TArray<FGameplayTag> StateChain = { BSGamePlayTags::InitState_Spawned,BSGamePlayTags::InitState_DataAvailable, BSGamePlayTags::InitState_DataInitialized,BSGamePlayTags::InitState_GameplayReady };
	ContinueInitStateChain(StateChain);
}

float UBSHealthComponent::GetHealth()
{
	return HealthSet->GetHealth();
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/BSPawnExtensionComponent.h"

#include "BSGamePlayTags.h"
#include "AbilitySystem/BSAbilitySystemComponent.h"
#include "Character/BSPawnData.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Net/UnrealNetwork.h"

const FName UBSPawnExtensionComponent::NAME_ActorFeatureName("PawnExtension");

UBSPawnExtensionComponent::UBSPawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

	PawnData = nullptr;
	AbilitySystemComponent = nullptr;
}

void UBSPawnExtensionComponent::OnRegister()
{
	Super::OnRegister();

	const APawn* Pawn = GetPawn<APawn>();
	ensureAlwaysMsgf((Pawn != nullptr), TEXT("LyraPawnExtensionComponent on [%s] can only be added to Pawn actors."), *GetNameSafe(GetOwner()));

	TArray<UActorComponent*> PawnExtensionComponents;
	Pawn->GetComponents(UBSPawnExtensionComponent::StaticClass(), PawnExtensionComponents);
	ensureAlwaysMsgf((PawnExtensionComponents.Num() == 1), TEXT("Only one LyraPawnExtensionComponent should exist on [%s]."), *GetNameSafe(GetOwner()));

	// Register with the init state system early, this will only work if this is a game world
	RegisterInitStateFeature();
}

void UBSPawnExtensionComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogBSInitState, Log, TEXT("UBSPawnExtensionComponent::BeginPlay"));
	
	// Listen for changes to all features
	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);

	UE_LOG(LogBSInitState, Log, TEXT("UBSPawnExtensionComponent::BeginPlay::TryToChangeInitState"));
	// Notifies state manager that we have spawned, then try rest of default initialization
	ensure(TryToChangeInitState(BSGamePlayTags::InitState_Spawned));
	
	UE_LOG(LogBSInitState, Log, TEXT("UBSPawnExtensionComponent::BeginPlay::CheckDefaultInitialization"));
	CheckDefaultInitialization();
}

void UBSPawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UninitializeAbilitySystem();
	UnregisterInitStateFeature();
	
	Super::EndPlay(EndPlayReason);
}

void UBSPawnExtensionComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBSPawnExtensionComponent, PawnData);
}

bool UBSPawnExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState,
	FGameplayTag DesiredState) const
{
	UE_LOG(LogBSInitState, Log, TEXT("UBSPawnExtensionComponent::CanChangeInitState"));
	UE_LOG(LogBSInitState, Log, TEXT("UBSPawnExtensionComponent::InitState changed: %s → %s"),
		*CurrentState.ToString(),
		*DesiredState.ToString());
	
	APawn* Pawn = GetPawn<APawn>();
	if (!CurrentState.IsValid() && DesiredState == BSGamePlayTags::InitState_Spawned)
	{
		// As long as we are on a valid pawn, we count as spawned
		if (Pawn)
		{
			return true;
		}
	}
	if (CurrentState == BSGamePlayTags::InitState_Spawned && DesiredState == BSGamePlayTags::InitState_DataAvailable)
	{
		// Pawn data is required.
		if (!PawnData)
		{
			return false;
		}

		const bool bHasAuthority = Pawn->HasAuthority();
		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();

		if (bHasAuthority || bIsLocallyControlled)
		{
			// Check for being possessed by a controller.
			if (!GetController<AController>())
			{
				return false;
			}
		}

		return true;
	}
	else if (CurrentState == BSGamePlayTags::InitState_DataAvailable && DesiredState == BSGamePlayTags::InitState_DataInitialized)
	{
		// Transition to initialize if all features have their data available
		return Manager->HaveAllFeaturesReachedInitState(Pawn, BSGamePlayTags::InitState_DataAvailable);
	}
	else if (CurrentState == BSGamePlayTags::InitState_DataInitialized && DesiredState == BSGamePlayTags::InitState_GameplayReady)
	{
		return true;
	}

	return false;
}

void UBSPawnExtensionComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager,
	FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	UE_LOG(LogBSInitState, Log, TEXT("UBSPawnExtensionComponent::HandleChangeInitState"));
	
	if (DesiredState == BSGamePlayTags::InitState_DataInitialized)
	{
		// This is currently all handled by other components listening to this state change
	}
}

void UBSPawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	UE_LOG(LogBSInitState, Log, TEXT("UBSPawnExtensionComponent::OnActorInitStateChanged"));
	UE_LOG(LogBSInitState, Log, TEXT("Actor %s InitState changed for feature %s: %s"),
	   *GetNameSafe(Params.OwningActor),
	   *Params.FeatureName.ToString(),
	   *Params.FeatureState.ToString());
	
	// If another feature is now in DataAvailable, see if we should transition to DataInitialized
	if (Params.FeatureName != NAME_ActorFeatureName)
	{
		if (Params.FeatureState == BSGamePlayTags::InitState_DataAvailable)
		{
			CheckDefaultInitialization();
		}
	}
}

/*
  필요한 다른 컴포넌트들이 준비됐는지 확인한 뒤,
  이 컴포넌트가 차례차례 Spawned → DataAvailable → DataInitialized → GameplayReady 단계로 올라갈 수 있도록 해주는 자동 진행 시스템 입니다.
 */
void UBSPawnExtensionComponent::CheckDefaultInitialization()
{
	// Before checking our progress, try progressing any other features we might depend on
	CheckDefaultInitializationForImplementers();

	static const TArray<FGameplayTag> StateChain = { BSGamePlayTags::InitState_Spawned, BSGamePlayTags::InitState_DataAvailable, BSGamePlayTags::InitState_DataInitialized, BSGamePlayTags::InitState_GameplayReady };

	// This will try to progress from spawned (which is only set in BeginPlay) through the data initialization stages until it gets to gameplay ready
	ContinueInitStateChain(StateChain);
}

void UBSPawnExtensionComponent::SetPawnData(const UBSPawnData* InPawnData)
{
	check(InPawnData);

	APawn* Pawn = GetPawnChecked<APawn>();

	if (Pawn->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (PawnData)
	{
		return;
	}

	PawnData = InPawnData;

	Pawn->ForceNetUpdate();

	CheckDefaultInitialization();
}

void UBSPawnExtensionComponent::OnRep_PawnData()
{
	CheckDefaultInitialization();
}

void UBSPawnExtensionComponent::InitializeAbilitySystem(UBSAbilitySystemComponent* InASC, AActor* InOwnerActor)
{
	check(InASC);
	check(InOwnerActor);

	if (AbilitySystemComponent == InASC)
	{
		// The ability system component hasn't changed.
		return;
	}

	if (AbilitySystemComponent)
	{
		// Clean up the old ability system component.
		UninitializeAbilitySystem();
	}

	APawn* Pawn = GetPawnChecked<APawn>();
	AActor* ExistingAvatar = InASC->GetAvatarActor();

	UE_LOG(LogBS, Verbose, TEXT("Setting up ASC [%s] on pawn [%s] owner [%s], existing [%s] "), *GetNameSafe(InASC), *GetNameSafe(Pawn), *GetNameSafe(InOwnerActor), *GetNameSafe(ExistingAvatar));

	if ((ExistingAvatar != nullptr) && (ExistingAvatar != Pawn))
	{
		UE_LOG(LogBS, Log, TEXT("Existing avatar (authority=%d)"), ExistingAvatar->HasAuthority() ? 1 : 0);

		// There is already a pawn acting as the ASC's avatar, so we need to kick it out
		// This can happen on clients if they're lagged: their new pawn is spawned + possessed before the dead one is removed
		ensure(!ExistingAvatar->HasAuthority());

		if (UBSPawnExtensionComponent* OtherExtensionComponent = FindPawnExtensionComponent(ExistingAvatar))
		{
			OtherExtensionComponent->UninitializeAbilitySystem();
		}
	}

	AbilitySystemComponent = InASC;
	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, Pawn);

	if (ensure(PawnData))
	{
		//InASC->SetTagRelationshipMapping(PawnData->TagRelationshipMapping);
	}

	//OnAbilitySystemInitialized.Broadcast();
}

void UBSPawnExtensionComponent::UninitializeAbilitySystem()
{
	
	if (!AbilitySystemComponent)
	{
		return;
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	if (AbilitySystemComponent->GetAvatarActor() == GetOwner())
	{
		FGameplayTagContainer AbilityTypesToIgnore;
		//AbilityTypesToIgnore.AddTag(BSGamePlayTags::Ability_Behavior_SurvivesDeath);

		AbilitySystemComponent->CancelAbilities(nullptr, &AbilityTypesToIgnore);
		//AbilitySystemComponent->ClearAbilityInput();
		AbilitySystemComponent->RemoveAllGameplayCues();

		if (AbilitySystemComponent->GetOwnerActor() != nullptr)
		{
			AbilitySystemComponent->SetAvatarActor(nullptr);
		}
		else
		{
			// If the ASC doesn't have a valid owner, we need to clear *all* actor info, not just the avatar pairing
			AbilitySystemComponent->ClearActorInfo();
		}

		//OnAbilitySystemUninitialized.Broadcast();
	}

	AbilitySystemComponent = nullptr;
}

void UBSPawnExtensionComponent::HandleControllerChanged()
{
	if (AbilitySystemComponent && (AbilitySystemComponent->GetAvatarActor() == GetPawnChecked<APawn>()))
	{
		ensure(AbilitySystemComponent->AbilityActorInfo->OwnerActor == AbilitySystemComponent->GetOwnerActor());
		if (AbilitySystemComponent->GetOwnerActor() == nullptr)
		{
			UninitializeAbilitySystem();
		}
		else
		{
			AbilitySystemComponent->RefreshAbilityActorInfo();
		}
	}

	CheckDefaultInitialization();
}

void UBSPawnExtensionComponent::HandlePlayerStateReplicated()
{
	CheckDefaultInitialization();
}

void UBSPawnExtensionComponent::SetupPlayerInputComponent()
{
	CheckDefaultInitialization();
}


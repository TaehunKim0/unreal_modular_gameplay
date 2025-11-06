// Fill out your copyright notice in the Description page of Project Settings.


#include "BSPawnInputComponent.h"
#include "BSGamePlayTags.h"
#include "BSLogChannels.h"
#include "BSPawnStateManagerComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/BSAbilitySystemComponent.h"
#include "Character/BSCharacter.h"
#include "Character/BSPawnData.h"
#include "Components/GameFrameworkComponentDelegates.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Core/BSCharacterDefinition.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Character.h"
#include "GameModes/BSAssetManager.h"
#include "GameModes/BSGameState.h"
#include "Input/BSInputComponent.h"
#include "Input/BSInputSet.h"
#include "Player/BSPlayerController.h"
#include "Player/BSPlayerState.h"

const FName UBSPawnInputComponent::NAME_PAWNINPUTCOMPONENT("PawnInputComponent");

UBSPawnInputComponent::UBSPawnInputComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UBSPawnInputComponent::OnRegister()
{
	Super::OnRegister();

	RegisterInitStateFeature();
}

void UBSPawnInputComponent::BeginPlay()
{
	Super::BeginPlay();

	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);

	ensure(TryToChangeInitState(BSGamePlayTags::InitState_Spawned));
	CheckDefaultInitialization();
}

void UBSPawnInputComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UnregisterInitStateFeature();

	UE_LOG(LogBS, Log, TEXT("UBSPawnInputComponent::EndPlay"));
}

void UBSPawnInputComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent, const UBSCharacterDefinition* InCharacterDef)
{
	check(PlayerInputComponent);

	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const APlayerController* PC = GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = Cast<ULocalPlayer>(PC->GetLocalPlayer());
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	if (const UBSInputSet* InputConfig = InCharacterDef->DefaultInputSet)
	{
	    UBSInputComponent* BSIC = Cast<UBSInputComponent>(PlayerInputComponent);
		
		// InputMappingContext
		if (const UInputMappingContext* IMC = InCharacterDef->DefaultInputMappingContext.Get())
		{
			Subsystem->AddMappingContext(IMC, 0);
		}

		// Bind Input Action
	    if (ensureMsgf(BSIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UBSInputComponent or a subclass of it.")))
	    {
		    TArray<uint32> BindHandles;
		    BSIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
			 
		    BSIC->BindNativeAction(InputConfig, BSGamePlayTags::FindTagByString("InputTag.Native.Move", false), ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
		    BSIC->BindNativeAction(InputConfig, BSGamePlayTags::FindTagByString("InputTag.Native.Look", false), ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
		    BSIC->BindNativeAction(InputConfig, BSGamePlayTags::FindTagByString("InputTag.Native.Look", false), ETriggerEvent::Triggered, this, &ThisClass::Input_LookStick, /*bLogIfNotFound=*/ false);
	    }
	}
}

void UBSPawnInputComponent::OnCharacterDefinitionChanged(const UBSCharacterDefinition* NewDefinition)
{
	CheckDefaultInitialization();
}

void UBSPawnInputComponent::AddAdditionalBindAction(const UBSInputSet* InInputSet,TArray<uint32>& OutBindHandles)
{
	UBSInputComponent* BSIC = Cast<UBSInputComponent>(GetPawn<APawn>()->FindComponentByClass<UInputComponent>());
	if (ensureMsgf(BSIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UBSInputComponent or a subclass of it.")))
	{
		BSIC->BindAbilityActions(InInputSet, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ OutBindHandles);
	}
}

void UBSPawnInputComponent::RemoveAdditionalBindAction(const uint32 InBindHandle)
{
	UBSInputComponent* BSIC = Cast<UBSInputComponent>(GetPawn<APawn>()->FindComponentByClass<UInputComponent>());
	if (ensureMsgf(BSIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UBSInputComponent or a subclass of it.")))
	{
		BSIC->RemoveBindingByHandle(InBindHandle);
	}
}

void UBSPawnInputComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (APawn* Pawn = GetPawn<APawn>())
	{
		const ABSCharacter* BSCharacter = Cast<ABSCharacter>(Pawn);
		if (UBSAbilitySystemComponent* ASC = BSCharacter->GetBSAbilitySystemComponent())
		{
			ASC->AbilityInputTagPressed(InputTag);
		}
	}
}

void UBSPawnInputComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (APawn* Pawn = GetPawn<APawn>())
	{
		const ABSCharacter* BSCharacter = Cast<ABSCharacter>(Pawn);
		if (UBSAbilitySystemComponent* ASC = BSCharacter->GetBSAbilitySystemComponent())
		{
			ASC->AbilityInputTagReleased(InputTag);
		}
	}
}

void UBSPawnInputComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	const AController* Controller = Pawn ? Pawn->GetController() : nullptr;

	if (Controller)
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (Value.X != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			Pawn->AddMovementInput(MovementDirection, Value.X);
		}

		if (Value.Y != 0.0f)
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Pawn->AddMovementInput(MovementDirection, Value.Y);
		}

		OnPawnMoveDelegate.Broadcast(Value);
	}
}

void UBSPawnInputComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y);
	}
}

void UBSPawnInputComponent::Input_LookStick(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();

	if (!Pawn)
	{
		return;
	}

	const FVector2D Value = InputActionValue.Get<FVector2D>();

	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X);
	}

	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y);
	}
}

//~ Begin IGameFrameworkInitStateInterface interface
//
//
void UBSPawnInputComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager,
	FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	UE_LOG(LogBSInitState, Log, TEXT("UBSPawnInputComponent::Actor State %s -> %s"), *CurrentState.ToString(), *DesiredState.ToString());
	
	if (CurrentState == BSGamePlayTags::InitState_CharacterDefinitionInitialized)
	{
		const APawn* Pawn = GetPawn<APawn>();
		const ABSPlayerState* BSPS = GetPlayerState<ABSPlayerState>();

		if (Pawn->InputComponent != nullptr)
		{
			InitializePlayerInput(Pawn->InputComponent, BSPS->GetCharacterDefData());
		}
	}
}

bool UBSPawnInputComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager,
	FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	if (DesiredState == BSGamePlayTags::InitState_CharacterDefinitionInitialized)
	{
		return Manager->HasFeatureReachedInitState(GetOwningActor(), UBSPawnStateManagerComponent::NAME_PAWNSTATEMANAGERCOMPONENT,BSGamePlayTags::InitState_CharacterDefinitionInitialized);
	}

	return true;
}

void UBSPawnInputComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName != NAME_PAWNINPUTCOMPONENT)
	{
		//UE_LOG(LogBSInitState, Log, TEXT("UBSPawnInputComponent::OnActorInitStateChanged"));
		CheckDefaultInitialization();
	}
}

void UBSPawnInputComponent::CheckDefaultInitialization()
{
	static const TArray<FGameplayTag> StateChain = { BSGamePlayTags::InitState_Spawned,BSGamePlayTags::InitState_PlayerStateInitialized, BSGamePlayTags::InitState_InputComponentInitialized, BSGamePlayTags::InitState_ASCInitialized, BSGamePlayTags::InitState_CharacterDefinitionInitialized, BSGamePlayTags::InitState_CharacterComponentInitialized, BSGamePlayTags::InitState_GameplayReady };
	ContinueInitStateChain(StateChain);
}
//
//
//~ End IGameFrameworkInitStateInterface interface
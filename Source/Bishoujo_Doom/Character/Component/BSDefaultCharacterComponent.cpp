// Fill out your copyright notice in the Description page of Project Settings.


#include "BSDefaultCharacterComponent.h"
#include "BSGamePlayTags.h"
#include "BSLogChannels.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/BSAbilitySystemComponent.h"
#include "Character/BSCharacter.h"
#include "Character/BSPawnData.h"
#include "Components/GameFrameworkComponentDelegates.h"
#include "Core/BSCharacterDefinition.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Character.h"
#include "GameModes/BSGameState.h"
#include "Input/BSInputComponent.h"
#include "Input/BSInputConfig.h"
#include "Player/BSPlayerController.h"
#include "Player/BSPlayerState.h"

const FName UBSDefaultCharacterComponent::NAME_DEFAULTCHARACTERCOMPONENT("DefaultCharacterComponent");

UBSDefaultCharacterComponent::UBSDefaultCharacterComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UBSDefaultCharacterComponent::OnRegister()
{
	Super::OnRegister();

	UE_LOG(LogBS, Log, TEXT("UBSDefaultCharacterComponent::OnRegister"));
	RegisterInitStateFeature();
}

void UBSDefaultCharacterComponent::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogBS, Log, TEXT("UBSDefaultCharacterComponent::BeginPlay"));

	BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);

	ABSGameState* BSGameState = GetWorld()->GetGameState<ABSGameState>();
	BSGameState->CharacterDefManagerComponent->OnCharacterDefinitionChangedDelegate.AddDynamic(this, &UBSDefaultCharacterComponent::OnCharacterDefinitionChanged);
	
	ensure(TryToChangeInitState(BSGamePlayTags::InitState_Spawned));
	CheckDefaultInitialization();
}

void UBSDefaultCharacterComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	UnregisterInitStateFeature();
}

void UBSDefaultCharacterComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager,
	FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	if (CurrentState == BSGamePlayTags::InitState_CharacterInitialized)
	{
		APawn* Pawn = GetPawn<APawn>();
		ABSPlayerState* BSPS = GetPlayerState<ABSPlayerState>();
		if (!ensure(Pawn && BSPS))
		{
			return;
		}

		if (ABSPlayerController* BSPC = GetController<ABSPlayerController>())
		{
			if (Pawn->InputComponent != nullptr)
			{
				InitializePlayerInput(Pawn->InputComponent);
			}
		}
	}
	
	UE_LOG(LogBSInitState, Log, TEXT("UBSDefaultCharacterComponent::HandleChangeInitState"));
	IGameFrameworkInitStateInterface::HandleChangeInitState(Manager, CurrentState, DesiredState);
}

bool UBSDefaultCharacterComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager,
	FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	UE_LOG(LogBSInitState, Log, TEXT("UBSDefaultCharacterComponent::CanChangeInitState"));
	
	if (CurrentState == BSGamePlayTags::InitState_CharacterDefinitionLoaded && DesiredState == BSGamePlayTags::InitState_CharacterInitialized)
	{
		UE_LOG(LogBSInitState, Log, TEXT("UBSDefaultCharacterComponent::InitState_CharacterDefinitionLoaded reached"));

		if (IsValid(GetPlayerState<ABSPlayerState>()->GetCharacterDefData()))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return true;
}

void UBSDefaultCharacterComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	if (Params.FeatureName != NAME_DEFAULTCHARACTERCOMPONENT)
	{
		UE_LOG(LogBSInitState, Log, TEXT("[UBSDefaultCharacterComponent] 다른 컴포넌트 상태 변화 감지: %s → %s"), 
			   *Params.FeatureName.ToString(), *Params.FeatureState.ToString());
        
		// 다른 컴포넌트가 변화하면 자신도 다음 단계 진행 시도
		CheckDefaultInitialization();
	}
}

void UBSDefaultCharacterComponent::CheckDefaultInitialization()
{
	static const TArray<FGameplayTag> StateChain = { BSGamePlayTags::InitState_Spawned, BSGamePlayTags::InitState_CharacterDefinitionLoaded, BSGamePlayTags::InitState_CharacterInitialized, BSGamePlayTags::InitState_GameplayReady };
	ContinueInitStateChain(StateChain);
}

void UBSDefaultCharacterComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	UE_LOG(LogBS, Log, TEXT("UBSDefaultCharacterComponent::InitializePlayerInput"));

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

	Subsystem->ClearAllMappings();

	const APlayerState* PS = PC->GetPlayerState<APlayerState>();
	if (!PS)
	{
		return;
	}

	const ABSPlayerState* BSPS = Cast<ABSPlayerState>(PS);
	if (!BSPS)
	{
		return;
	}

	if (const UBSPawnData* PawnData = BSPS->GetCharacterDefData()->PawnData)
    {
       if (const UBSInputConfig* InputConfig = PawnData->InputConfig)
       {
          if (UInputMappingContext* IMC = InputMappingContext.Get())
          {
             Subsystem->AddMappingContext(IMC, 0);
          	 UE_LOG(LogBS, Log, TEXT("UBSDefaultCharacterComponent::InitializePlayerInput::AddMappingContext"));
          }
    
          UBSInputComponent* BSIC = Cast<UBSInputComponent>(PlayerInputComponent);
          if (ensureMsgf(BSIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to UBSInputComponent or a subclass of it.")))
          {
             TArray<uint32> BindHandles;
             BSIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);
			 
             BSIC->BindNativeAction(InputConfig, BSGamePlayTags::FindTagByString("InputTag.Native.Move", false), ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
             BSIC->BindNativeAction(InputConfig, BSGamePlayTags::FindTagByString("InputTag.Native.Look", false), ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
             BSIC->BindNativeAction(InputConfig, BSGamePlayTags::FindTagByString("InputTag.Native.Look", false), ETriggerEvent::Triggered, this, &ThisClass::Input_LookStick, /*bLogIfNotFound=*/ false);

          	UE_LOG(LogBS, Log, TEXT("UBSDefaultCharacterComponent::InitializePlayerInput::BindNativeAction"));
          }
       }
    }
}

void UBSDefaultCharacterComponent::OnCharacterDefinitionChanged(const UBSCharacterDefinition* NewDefinition)
{
	CheckDefaultInitialization();
}

void UBSDefaultCharacterComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
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

void UBSDefaultCharacterComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
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

void UBSDefaultCharacterComponent::Input_Move(const FInputActionValue& InputActionValue)
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
	}
}

void UBSDefaultCharacterComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
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

void UBSDefaultCharacterComponent::Input_LookStick(const FInputActionValue& InputActionValue)
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


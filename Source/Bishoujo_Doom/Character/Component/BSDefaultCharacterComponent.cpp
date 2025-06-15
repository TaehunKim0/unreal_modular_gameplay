// Fill out your copyright notice in the Description page of Project Settings.


#include "BSDefaultCharacterComponent.h"
#include "BSGamePlayTags.h"
#include "BSLogChannels.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayTagContainer.h"
#include "Character/BSCharacterDefinition.h"
#include "Character/BSPawnData.h"
#include "Input/BSInputComponent.h"
#include "Input/BSInputConfig.h"
#include "Player/BSPlayerController.h"
#include "Player/BSPlayerState.h"

void UBSDefaultCharacterComponent::OnRegister()
{
	Super::OnRegister();

	UE_LOG(LogBS, Log, TEXT("UBSDefaultCharacterComponent::OnRegister"));
}

void UBSDefaultCharacterComponent::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogBS, Log, TEXT("UBSDefaultCharacterComponent::BeginPlay"));
}

void UBSDefaultCharacterComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UBSDefaultCharacterComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager,
	FGameplayTag CurrentState, FGameplayTag DesiredState)
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
	
	UE_LOG(LogBS, Log, TEXT("UBSDefaultCharacterComponent::HandleChangeInitState"));
	IGameFrameworkInitStateInterface::HandleChangeInitState(Manager, CurrentState, DesiredState);
}

bool UBSDefaultCharacterComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager,
	FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	return true;
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

	if (const UBSPawnData* PawnData = BSPS->GetCharacterDefData<UBSCharacterDefinition>()->DefaultPawnData)
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
    
             BSIC->BindNativeAction(InputConfig, BSGamePlayTags::FindTagByString("InputTag_Native_Move", false), ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
             BSIC->BindNativeAction(InputConfig, BSGamePlayTags::FindTagByString("InputTag_Native_Look", false), ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
             BSIC->BindNativeAction(InputConfig, BSGamePlayTags::FindTagByString("InputTag_Native_Look", false), ETriggerEvent::Triggered, this, &ThisClass::Input_LookStick, /*bLogIfNotFound=*/ false);

          	UE_LOG(LogBS, Log, TEXT("UBSDefaultCharacterComponent::InitializePlayerInput::BindNativeAction"));
          }
       }
    }
}

void UBSDefaultCharacterComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
}

void UBSDefaultCharacterComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
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


// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Core/BSCharacterDefinition.h"
#include "UI/Debug/BSDebugWidget.h"
#include "BSPawnInputComponent.generated.h"

class UBSInputSet;
struct FInputActionValue;
struct FGameplayTag;
class UInputMappingContext;
class UGameFrameworkComponentManager;
/**
 * 
 */

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPawnMoveDelegate, const FVector2D& MovementValue);

UCLASS()
class BISHOUJO_DOOM_API UBSPawnInputComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	UBSPawnInputComponent(const  FObjectInitializer& ObjectInitializer);
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	//~ Begin IGameFrameworkInitStateInterface interface
	virtual FName GetFeatureName() const override { return NAME_PAWNINPUTCOMPONENT; }
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	//~ End IGameFrameworkInitStateInterface interface
	
	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent, const UBSCharacterDefinition* InCharacterDef);
	UFUNCTION()
	void OnCharacterDefinitionChanged(const UBSCharacterDefinition* NewDefinition);

public:
	void AddAdditionalBindAction(const UBSInputSet* InInputSet, TArray<uint32>& OutBindHandles);
	void RemoveAdditionalBindAction(const uint32 InBindHandle);
	
public:
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);
	void Input_LookStick(const FInputActionValue& InputActionValue);
	
public:
	static const FName NAME_PAWNINPUTCOMPONENT;
	FOnPawnMoveDelegate OnPawnMoveDelegate;
};


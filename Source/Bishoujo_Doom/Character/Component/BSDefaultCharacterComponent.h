// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "BSDefaultCharacterComponent.generated.h"

struct FInputActionValue;
struct FGameplayTag;
class UInputMappingContext;
class UGameFrameworkComponentManager;
/**
 * 
 */

UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class BISHOUJO_DOOM_API UBSDefaultCharacterComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	virtual void OnRegister() override;

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//~ Begin IGameFrameworkInitStateInterface interface
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	//~ End IGameFrameworkInitStateInterface interface
	
	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);
	
public:
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);
	void Input_LookStick(const FInputActionValue& InputActionValue);

public:
	UPROPERTY(EditAnywhere, Category="Input", meta=(AssetBundles="Client,Server"))
	TSoftObjectPtr<UInputMappingContext> InputMappingContext;
};


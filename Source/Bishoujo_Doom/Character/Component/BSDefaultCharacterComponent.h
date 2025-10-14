// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Core/BSCharacterDefinition.h"
#include "BSDefaultCharacterComponent.generated.h"

struct FInputActionValue;
struct FGameplayTag;
class UInputMappingContext;
class UGameFrameworkComponentManager;
/**
 * 
 */

UCLASS()
class BISHOUJO_DOOM_API UBSDefaultCharacterComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	UBSDefaultCharacterComponent(const  FObjectInitializer& ObjectInitializer);
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	//~ Begin IGameFrameworkInitStateInterface interface
	virtual FName GetFeatureName() const override { return NAME_DEFAULTCHARACTERCOMPONENT; }
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	//~ End IGameFrameworkInitStateInterface interface
	
	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);
	UFUNCTION()
	void OnCharacterDefinitionChanged(const UBSCharacterDefinition* NewDefinition);
	
public:
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);
	void Input_LookStick(const FInputActionValue& InputActionValue);

	UPROPERTY(EditAnywhere, Category="Input", meta=(AssetBundles="Client,Server"))
	TSoftObjectPtr<UInputMappingContext> InputMappingContext;

private:
	static const FName NAME_DEFAULTCHARACTERCOMPONENT;
};


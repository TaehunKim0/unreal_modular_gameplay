// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "BSPawnStateManagerComponent.generated.h"

class UBSCharacterDefinition;
class UBSAbilitySystemComponent;
class UBSPawnData;
/**
 * 
 */
UCLASS()
class BISHOUJO_DOOM_API UBSPawnStateManagerComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	UBSPawnStateManagerComponent(const FObjectInitializer& ObjectInitializer);

	//~ Begin IGameFrameworkInitStateInterface interface
	virtual FName GetFeatureName() const override { return NAME_PAWNSTATEMANAGERCOMPONENT; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	//~ End IGameFrameworkInitStateInterface interface

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnCharacterDefinitionChanged(const ABSPlayerState* InPlayerState, const UBSCharacterDefinition* InNewDefinition);

	UFUNCTION()
	void OnPawnRestarted(APawn* NewPawn);

	UFUNCTION()
	void OnPlayerStateChanged(APlayerState* NewPlayerState);

public:
	FSimpleMulticastDelegate OnAbilitySystemInitialized;
	FSimpleMulticastDelegate OnAbilitySystemUninitialized;
	FSimpleMulticastDelegate OnPawnGameplayReadyCompleted;

public:
	static const FName NAME_PAWNSTATEMANAGERCOMPONENT;
};

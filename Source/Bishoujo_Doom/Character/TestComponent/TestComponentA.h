// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "GameplayTagContainer.h"
#include "TestComponentA.generated.h"

/**
 * 
 */
UCLASS()
class BISHOUJO_DOOM_API UTestComponentA : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	UTestComponentA(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// IGameFrameworkInitStateInterface
	virtual FName GetFeatureName() const override { return NAME_TestComponentA; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	static const FName NAME_TestComponentA;

	UPROPERTY()
	bool bDataLoaded = false;

	void LoadMyData();
	void InitializeMySystem();
};
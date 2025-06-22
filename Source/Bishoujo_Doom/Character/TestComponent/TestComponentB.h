// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "GameplayTagContainer.h"
#include "TestComponentB.generated.h"

UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class BISHOUJO_DOOM_API UTestComponentB : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	UTestComponentB(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// IGameFrameworkInitStateInterface
	virtual FName GetFeatureName() const override { return NAME_TestComponentB; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	static const FName NAME_TestComponentB;

	UPROPERTY()
	bool bConfigLoaded = false;

	void LoadConfiguration();
	void SetupMyFeatures();
};

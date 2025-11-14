// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "BSAbilitySystemComponent.generated.h"



/**
 * 
 */
UCLASS()
class BISHOUJO_DOOM_API UBSAbilitySystemComponent : public UAbilitySystemComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

	UBSAbilitySystemComponent();

public:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;

	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

	//~ Begin IGameFrameworkInitStateInterface interface
	virtual FName GetFeatureName() const override { return UBSAbilitySystemComponent::NAME_ABILITYSYSTEMCOMPONENT; }
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	//~ End IGameFrameworkInitStateInterface interface

public:
	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

protected:
	// Handles to abilities that had their input pressed this frame.
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	// Handles to abilities that had their input released this frame.
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

	// Handles to abilities that have their input held.
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

public:
	static const FName NAME_ABILITYSYSTEMCOMPONENT;
};

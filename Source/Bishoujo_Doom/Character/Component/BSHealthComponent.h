// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "Components/GameFrameworkComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "BSHealthComponent.generated.h"

class UBSHealthAttributeSet;
class UBSAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBSHealth_DeathEvent, AActor*, OwningActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FBSHealth_AttributeChanged, UBSHealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);

/**
 * 
 */
UCLASS()
class BISHOUJO_DOOM_API UBSHealthComponent : public UGameFrameworkComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	UBSHealthComponent(const FObjectInitializer& ObjectInitializer);

	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	//~ Begin IGameFrameworkInitStateInterface interface
	virtual FName GetFeatureName() const override { return NAME_HEALTHCOMPONENT; }
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	//~ End IGameFrameworkInitStateInterface interface

	float GetHealth();

protected:
	void InitializeWithAbilitySystem(UBSAbilitySystemComponent* InAbilitySystem);
	
	virtual void HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);

public:
	static const FName NAME_HEALTHCOMPONENT;
	
private:
	UPROPERTY(BlueprintAssignable)
	FBSHealth_AttributeChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FBSHealth_AttributeChanged OnMaxHealthChanged;

	UPROPERTY()
	TObjectPtr<UBSAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<const UBSHealthAttributeSet> HealthSet;
};

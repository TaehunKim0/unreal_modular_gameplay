// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ModularPlayerState.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "GameFramework/PlayerState.h"
#include "BSPlayerState.generated.h"

class UBSHealthAttributeSet;
class UBSAbilitySystemComponent;
class UBSCharacterDefinition;
class ABSPlayerController;
class UBSPawnData;

/**
 * 
 */
UCLASS()
class BISHOUJO_DOOM_API ABSPlayerState : public AModularPlayerState, public IAbilitySystemInterface, public IGameFrameworkInitStateInterface 
{
	GENERATED_BODY()
public:
	ABSPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~AActor interface
	virtual void PreInitializeComponents() override;
	UFUNCTION()
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(class TArray<class FLifetimeProperty,class TSizedDefaultAllocator<32> > &)const override;
	//~End of AActor interface

	//~ Begin IGameFrameworkInitStateInterface interface
	virtual FName GetFeatureName() const override { return NAME_PLAYERSTATE; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	//~ End IGameFrameworkInitStateInterface interface
	
public:
	UFUNCTION(BlueprintCallable, Category = "BS|PlayerState")
	UBSAbilitySystemComponent* GetBSAbilitySystemComponent() const { return AbilitySystemComponent; }
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	const UBSCharacterDefinition* GetCharacterDefData() const { return CharacterDefData; }
	void SetCharacterDefData(const UBSCharacterDefinition* InCharacterDefData);

	UFUNCTION(BlueprintCallable, Category = "BS|PlayerState")
	ABSPlayerController* GetBSPlayerController() const;

protected:
	void InitWithPawn();

public:
	static const FName NAME_PLAYERSTATE;
	
protected:
	UPROPERTY()
	TObjectPtr<const UBSCharacterDefinition> CharacterDefData;

	UPROPERTY()
	TObjectPtr<const UBSHealthAttributeSet> HealthAttributeSet; 

private:
	UPROPERTY(VisibleAnywhere, Category = "PlayerState")
	TObjectPtr<UBSAbilitySystemComponent> AbilitySystemComponent;
};
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "BSPawnExtensionComponent.generated.h"

class UBSAbilitySystemComponent;
class UBSPawnData;
/**
 * 
 */
UCLASS()
class BISHOUJO_DOOM_API UBSPawnExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	UBSPawnExtensionComponent(const FObjectInitializer& ObjectInitializer);

	/** The name of this overall feature, this one depends on the other named component features */
	static const FName NAME_ActorFeatureName;

	//~ Begin IGameFrameworkInitStateInterface interface
	virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;
	//~ End IGameFrameworkInitStateInterface interface

	UFUNCTION(BlueprintPure, Category = "BS|Pawn")
	static UBSPawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UBSPawnExtensionComponent>() : nullptr); }

public:
	UFUNCTION(BlueprintPure, Category = "BS|Pawn")
	UBSAbilitySystemComponent* GetBSAbilitySystemComponent() const { return AbilitySystemComponent; }

	void InitializeAbilitySystem(UBSAbilitySystemComponent* InASC, AActor* InOwnerActor);
	void UninitializeAbilitySystem();
	
	void HandleControllerChanged();
	void HandlePlayerStateReplicated();
	
	void SetupPlayerInputComponent();

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnCharacterDefinitionChanged(const UBSCharacterDefinition* InNewDefinition);

private:
	/** Pointer to the ability system component that is cached for convenience. */
	UPROPERTY()
	TObjectPtr<UBSAbilitySystemComponent> AbilitySystemComponent;
};

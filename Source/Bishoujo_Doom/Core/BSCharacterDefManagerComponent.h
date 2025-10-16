// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/GameStateComponent.h"
#include "GameFeatures/GameFeatureAction_AddWidgets.h"
#include "BSCharacterDefManagerComponent.generated.h"

enum EUICategory : uint8;
class UGameFeatureAction;
class UBSCharacterDefinition;
class ABSPlayerState;

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterDefinitionChanged,
	const UBSCharacterDefinition*, NewDefinition);

UCLASS()
class BISHOUJO_DOOM_API UBSCharacterDefManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	UBSCharacterDefManagerComponent(const FObjectInitializer& ObjectInitializer);
	
	void SetCharacterDefinition(APlayerState* InPlayerState, FGameplayTag InTag);
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:
	// Definition
	void ApplyCharacterDefinition(ABSPlayerState* InPlayerState, const UBSCharacterDefinition* NewCharacterDef);
	void ApplyPawnData(const ABSPlayerState* InPlayerState, const UBSCharacterDefinition* NewCharacterDef) const;
	void CleanupCharacterDefinition(ABSPlayerState* PlayerState, const UBSCharacterDefinition* OldCharacterDef);
	
	// GameFeature
	void EnableGameFeatures(ABSPlayerState* PlayerState, const TArray<FString>& GameFeaturesNameToEnable, const UBSCharacterDefinition* NewCharacterDef);
	void DisableGameFeatures(const TArray<FString>& GameFeaturesToDisable);
	void DisableAllGameFeatures();

	UFUNCTION()
	void OnCharacterDefinitionChanged(const UBSCharacterDefinition* InNewDefinition);

public:
	FOnCharacterDefinitionChanged OnCharacterDefinitionChangedDelegate;

private:
	TArray<FString> ActivatedGameFeatureNameArray;
};
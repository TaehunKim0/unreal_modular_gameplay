// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/GameStateComponent.h"
#include "GameFeatureAction/GameFeatureAction_AddWidgets.h"
#include "BSCharacterDefSystem.generated.h"

class UBSDebugWidget;
enum EUICategory : uint8;
class UGameFeatureAction;
class UBSCharacterDefinition;
class ABSPlayerState;

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCharacterDefinitionChanged,
	const ABSPlayerState*, InBSPlayerState , const UBSCharacterDefinition*, NewCharacterDef);

UCLASS()
class BISHOUJO_DOOM_API UBSCharacterDefSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UBSCharacterDefSystem();
	
	void SetCharacterDefinition(APlayerState* InPlayerState, FGameplayTag InTag);

	UFUNCTION(BlueprintCallable)
	void K2_SetCharacterDefinition(APlayerState* InPlayerState, FGameplayTag InTag);

protected:
	virtual void BeginDestroy() override;

	UFUNCTION()
	void OnActionDeactivationCompleted();
	
private:
	// Definition
	void ApplyCharacterDefinition(ABSPlayerState* InPlayerState, const UBSCharacterDefinition* NewCharacterDef);
	void ApplyPawnData(const ABSPlayerState* InPlayerState, const UBSCharacterDefinition* NewCharacterDef);
	void ApplyGameFeatureAction(const UBSCharacterDefinition* NewCharacterDef);

	void ClearASC(const ABSPlayerState* InPlayerState);
	bool RespawningPawn(const ABSPlayerState* InPlayerState, const UBSCharacterDefinition* NewCharacterDef);
	
	void CleanupCharacterDefinition(ABSPlayerState* InPlayerState, const UBSCharacterDefinition* OldCharacterDef);
	void DisableGameFeatureActions(const UBSCharacterDefinition* OldCharacterDef);
	
	// GameFeature
	void EnableGameFeatures(ABSPlayerState* InPlayerState, const TArray<FString>& InGameFeaturesNameToEnable, const UBSCharacterDefinition* NewCharacterDef);
	void DisableGameFeatures(const TArray<FString>& InGameFeaturesToDisable);

	UFUNCTION()
	void OnCharacterDefinitionChanged(const ABSPlayerState* InBSPlayerState, const UBSCharacterDefinition* InNewDefinition);

public:
	FOnCharacterDefinitionChanged OnCharacterDefinitionChangedDelegate;

private:

	struct PendingCharacterDefinition
	{
		APlayerState* PlayerState;
		FGameplayTag Tag;
	};
	TArray<PendingCharacterDefinition> PendingCharacterDefinitionArray;
	
	int32 RequiredEnableCount;
	bool bIsCharacterDefinitionLoading;
};
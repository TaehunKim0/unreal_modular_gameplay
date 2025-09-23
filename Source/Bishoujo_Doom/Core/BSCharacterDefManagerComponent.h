// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/GameStateComponent.h"
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
	// 새 캐릭터 정의 적용
	void ApplyCharacterDefinition(ABSPlayerState* PlayerState, const UBSCharacterDefinition* NewCharacterDef);
	// 이전 캐릭터 정의 정리
	void CleanupCharacterDefinition(ABSPlayerState* PlayerState, const UBSCharacterDefinition* OldCharacterDef);
	
	// GameFeature 관리
	void CollectGameFeaturePluginURL(const TArray<FString>& InFeaturePluginURLArray);
	void EnableGameFeatures(ABSPlayerState* PlayerState, const TArray<FString>& GameFeaturesToEnable, const UBSCharacterDefinition* NewCharacterDef) const;
	void DisableGameFeatures(ABSPlayerState* PlayerState, const TArray<FString>& GameFeaturesToDisable);
	void DisableAllGameFeatures();

	// GameAbilitySystem 관리
	void GiveAbilities(ABSPlayerState* PlayerState,const UBSCharacterDefinition* InCharacterDef);

	UFUNCTION()
	void OnCharacterDefinitionChanged(const UBSCharacterDefinition* InNewDefinition);

public:
	FOnCharacterDefinitionChanged OnCharacterDefinitionChangedDelegate;

private:
	TArray<FString> GameFeaturePluginURLs;
};
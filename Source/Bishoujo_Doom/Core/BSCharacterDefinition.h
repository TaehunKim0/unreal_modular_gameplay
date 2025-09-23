// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BSCharacterDefinition.generated.h"

class UBSAbilitySet;
class UBSPawnData;
class UGameFeatureAction;
/**
 * Pawn : 플레이어 또는 AI 용
 * Character : 이족 보행 플레이어용
 */
UCLASS()
class BISHOUJO_DOOM_API UBSCharacterDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UBSCharacterDefinition();
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "UBSCharacterDefinition")
	FString CharacterName;
	
	UPROPERTY(EditDefaultsOnly, Category = "UBSCharacterDefinition")
	TArray<FString> GameFeaturesToEnable;

	UPROPERTY(EditDefaultsOnly, Instanced, Category="UBSCharacterDefinition")
	TArray<TObjectPtr<UGameFeatureAction>> DefaultGameFeatureActions;
	
	UPROPERTY(EditDefaultsOnly, Category="UBSCharacterDefinition")
	TObjectPtr<const UBSPawnData> DefaultPawnData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UBSCharacterDefinition")
	TArray<TObjectPtr<UBSAbilitySet>> AbilitySets;
};
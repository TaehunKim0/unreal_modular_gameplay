// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BSCharacterDefinition.generated.h"

class UBSPawnData;
class UGameFeatureAction;
/**
 * 
 */
UCLASS()
class BISHOUJO_DOOM_API UBSCharacterDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UBSCharacterDefinition();
	
public:
	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TArray<FString> GameFeaturesToEnable;
	
	UPROPERTY(EditDefaultsOnly, Category=Gameplay)
	TObjectPtr<const UBSPawnData> DefaultPawnData;
	
	UPROPERTY(EditDefaultsOnly, Instanced, Category="Actions")
	TArray<TObjectPtr<UGameFeatureAction>> DefaultGameFeatureActions;
};
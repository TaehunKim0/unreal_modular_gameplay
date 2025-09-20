// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BSPawnData.generated.h"

class UCameraComponent;
class UBSInputConfig;
class UBSAbilitySet;
/**
 * 
 */
UCLASS()
class BISHOUJO_DOOM_API UBSPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UBSPawnData(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BS|Pawn")
	TSubclassOf<APawn> PawnClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BS|Input")
	TObjectPtr<UBSInputConfig> InputConfig;
};
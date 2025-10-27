// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "BSItemDefinition.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class ITEMSHOPRUNTIME_API UBSItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Item", ItemTag.GetTagLeafName());
	}

public:
	UPROPERTY(EditDefaultsOnly, Category = "UBSItemDefinition")
	FGameplayTag ItemTag;
};
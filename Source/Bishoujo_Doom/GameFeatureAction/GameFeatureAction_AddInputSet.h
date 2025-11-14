// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction.h"
#include "GameFeatureAction_AddInputSet.generated.h"

class UInputMappingContext;
class UBSInputSet;

USTRUCT(BlueprintType)
struct FGameFeatureInputSetEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="InputSet")
	TSoftClassPtr<AActor> TargetActorClass;

	UPROPERTY(EditAnywhere, Category="InputSet", meta=(AssetBundles="Client,Server"))
	TSoftObjectPtr<const UBSInputSet> GrantInputSet;
	
};
/**
 * 
 */
UCLASS(meta = (DisplayName = "Add InputSet"))
class BISHOUJO_DOOM_API UGameFeatureAction_AddInputSet : public UGameFeatureAction
{
	GENERATED_BODY()

public:
	//~ Begin UGameFeatureAction interface
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	//~ End UGameFeatureAction interface

public:
	void AddActorInputSet(APawn* InPawn, const FGameFeatureInputSetEntry& InInputSetEntry);
	void RemoveActorInputSet(APawn* InPawn);

	UPROPERTY(EditAnywhere)
	TArray<FGameFeatureInputSetEntry> InputSetArray;

	TMap<APawn*, TArray<uint32>> AddedInputSetMap;
};

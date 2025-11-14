// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction.h"
#include "GameFeatureAction_AddInputContextMapping.generated.h"

class UInputMappingContext;

USTRUCT()
struct FInputMappingSet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="InputContextMapping", meta=(AssetBundles="Client,Server"))
	TSoftObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditAnywhere, Category="InputContextMapping")
	uint8 Priority = 0;
};

USTRUCT(BlueprintType)
struct FGameFeatureInputContextMappingEntry
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, Category="InputContextMapping")
	TSoftClassPtr<AActor> TargetActorClass;

	UPROPERTY(EditAnywhere, Category="InputContextMapping")
	TArray<FInputMappingSet> GrantInputMappingArray;
};

/**
 * 
 */
UCLASS(meta = (DisplayName = "Add Input Context Mapping"))
class BISHOUJO_DOOM_API UGameFeatureAction_AddInputContextMapping final: public UGameFeatureAction
{
	GENERATED_BODY()

public:
	//~UGameFeatureAction interface
	virtual void OnGameFeatureRegistering() override;
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	virtual void OnGameFeatureUnregistering() override;
	//~End of UGameFeatureAction interface

public:
	void AddInputMappingForPlayer(APawn* InPawn);
	void RemoveInputMapping(APawn* InPawn);

protected:
	UPROPERTY(EditAnywhere, Category="InputContextMapping")
	FGameFeatureInputContextMappingEntry InputMapping;

	TMap<APawn*, TArray<UInputMappingContext*>> AddedInputMappingMap;
};
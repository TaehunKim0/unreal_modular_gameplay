// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction.h"
#include "GameFeatureAction_AddAttributes.generated.h"

class UAttributeSet;

USTRUCT()
struct FGameFeatureAttributesEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Attributes")
	TSoftClassPtr<AActor> TargetActorClass;

	UPROPERTY(EditAnywhere, Category = "Attributes")
	TArray<TSoftClassPtr<UAttributeSet>> AttributeSets;
};

/**
 * 
 */
UCLASS(meta = (DisplayName = "Add Attributes"))
class BISHOUJO_DOOM_API UGameFeatureAction_AddAttributes : public UGameFeatureAction
{
	GENERATED_BODY()

public:
	//~ Begin UGameFeatureAction interface
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	//~ End UGameFeatureAction interface

public:
	void AddActorAttributes(AActor* InActor, const FGameFeatureAttributesEntry& InAttributesEntry);
	void RemoveActorAttributes(const AActor* InActor);

public:
	UPROPERTY(EditAnywhere, Category = "Attributes", meta = (TitleProperty = "TargetActorClass", ShowOnlyInnerProperties))
	TArray<FGameFeatureAttributesEntry> AttributesArray;

private:
	TMap<AActor*, TArray<UAttributeSet*>> AddedAttributesMap;
};

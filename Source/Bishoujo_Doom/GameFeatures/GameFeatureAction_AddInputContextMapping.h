// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction.h"
#include "GameFeatureAction_AddInputContextMapping.generated.h"

class UInputMappingContext;
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
	
	UPROPERTY(EditAnywhere, Category="Input")
	TArray<TSoftObjectPtr<UInputMappingContext>> InputMappings;

	void AddInputMappingForPlayer(UPlayer* Player);
	void RemoveInputMapping(APlayerController* PlayerController);
};
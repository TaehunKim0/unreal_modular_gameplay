// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameFeatureAction.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameFeatureAction_AddAbilities.generated.h"

class UAttributeSet;
class UBSAbilitySet;
struct FGameplayAbilitySpecHandle;
class UGameplayAbility;

USTRUCT(BlueprintType)
struct FGameFeatureAbilitiesEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Abilities")
	TSoftClassPtr<AActor> TargetActorClass;

	UPROPERTY(EditAnywhere, Category="Attributes", meta=(AssetBundles="Client,Server"))
	TArray<TSoftObjectPtr<const UBSAbilitySet>> GrantAbilitySets;
};

/**
 * 
 */
UCLASS(meta = (DisplayName = "Add AbilitySet"))
class BISHOUJO_DOOM_API UGameFeatureAction_AddAbilities : public UGameFeatureAction
{
	GENERATED_BODY()

public:
	//~ Begin UGameFeatureAction interface
	UGameFeatureAction_AddAbilities(const FObjectInitializer& ObjectInitializer);
	
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	//~ End UGameFeatureAction interface

public:
	void AddActorAbilities(AActor* InActor, const FGameFeatureAbilitiesEntry& InAbilitiesEntry);
	void RemoveActorAbilities(AActor* InActor);

public:
	UPROPERTY(EditAnywhere)
	TArray<FGameFeatureAbilitiesEntry> AbilitiesArray;

private:
	struct FAbilitiesHandle
	{
		TArray<FGameplayAbilitySpecHandle> AbilitySpecArray;
		TArray<FActiveGameplayEffectHandle> GameplayEffectHandleArray;
		TArray<UAttributeSet*> AttributeSetArray;
	};
	
	TMap<AActor*, FAbilitiesHandle> AddedAbilitiesMap;

};

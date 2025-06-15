// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "BSAbilitySet.generated.h"

class UAttributeSet;
class UBSAbilitySystemComponent;
class UGameplayEffect;
class UGameplayAbility;
/**
 * 
 */
USTRUCT(BlueprintType)
struct FBSAbilitySet_GameplayAbility
{
	GENERATED_BODY()

public:
	// Gameplay ability to grant.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayAbility> Ability = nullptr;

	// Level of ability to grant.
	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;

	// Tag used to process input for the ability.
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};


USTRUCT(BlueprintType)
struct FBSAbilitySet_GameplayEffect
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;

	UPROPERTY(EditDefaultsOnly)
	float EffectLevel = 1.0f;
};

USTRUCT(BlueprintType)
struct FBSAbilitySet_AttributeSet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAttributeSet> AttributeSet;

};

UCLASS()
class BISHOUJO_DOOM_API UBSAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UBSAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void GiveToAbilitySystem(UBSAbilitySystemComponent* LyraASC, UObject* SourceObject = nullptr) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", meta=(TitleProperty=Ability))
	TArray<FBSAbilitySet_GameplayAbility> GrantedGameplayAbilitiesWithTag;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects", meta=(TitleProperty=GameplayEffect))
	TArray<FBSAbilitySet_GameplayEffect> GrantedGameplayEffects;

	UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets", meta=(TitleProperty=AttributeSet))
	TArray<FBSAbilitySet_AttributeSet> GrantedAttributes;
};
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "BSInputConfig.generated.h"

/**
 * 
 */
class UGameplayAbility;
class UInputAction;

USTRUCT(BlueprintType)
struct FBSInputAction
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

/**
 * 
 */
UCLASS(BlueprintType, Const)
class BISHOUJO_DOOM_API UBSInputConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "BS|Pawn")
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

	UFUNCTION(BlueprintCallable, Category = "BS|Pawn")
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FBSInputAction> NativeInputActions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FBSInputAction> AbilityInputActions;
};
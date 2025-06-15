// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "BSAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class BISHOUJO_DOOM_API UBSAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

	UBSAbilitySystemComponent();

	virtual void OnRegister() override;
	
};

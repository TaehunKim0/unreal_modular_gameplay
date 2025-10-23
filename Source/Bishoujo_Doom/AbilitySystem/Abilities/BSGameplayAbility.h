// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "BSGameplayAbility.generated.h"


class ABSCharacter;

UENUM(BlueprintType)
enum class EBSAbilityActivationPolicy : uint8
{
	// Try to activate the ability when the input is triggered.
	InputTriggeredOnce,

	// Continually try to activate the ability while the input is active.
	WhileInputActive,

	// Try to activate the ability when an avatar is assigned.
	OnSpawn
};

/**
 * 
 */
UCLASS()
class BISHOUJO_DOOM_API UBSGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	friend class UBSAbilitySystemComponent;

public:
	EBSAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }

	ABSCharacter* GetBSCharacterFromActorInfo() const;
	ACharacter* GetCharacterFromActorInfo() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Activation")
	EBSAbilityActivationPolicy ActivationPolicy;
};

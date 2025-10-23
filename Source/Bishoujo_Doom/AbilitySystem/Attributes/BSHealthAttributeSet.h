// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Attributes/BSAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "BSHealthAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class BISHOUJO_DOOM_API UBSHealthAttributeSet : public UBSAttributeSet
{
	GENERATED_BODY()

public:
	UBSHealthAttributeSet();

	ATTRIBUTE_ACCESSORS(UBSHealthAttributeSet, Health);
	ATTRIBUTE_ACCESSORS(UBSHealthAttributeSet, MaxHealth);
	ATTRIBUTE_ACCESSORS(UBSHealthAttributeSet, Healing);
	ATTRIBUTE_ACCESSORS(UBSHealthAttributeSet, Damage);

	mutable FBSAttributeEvent OnHealthChanged;
	mutable FBSAttributeEvent OnMaxHealthChanged;

protected:
	// Begin UAttributeSet~
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	// ~End UAttributeSet

	// Begin ~
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	// ~End
	
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

	
private:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	UPROPERTY(BlueprintReadOnly, Category="Health", Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData Healing;

	UPROPERTY(BlueprintReadOnly, Category="Health", Meta=(AllowPrivateAccess=true))
	FGameplayAttributeData Damage;

	float HealthBeforeAttributeChange;
	float MaxHealthBeforeAttributeChange;
};

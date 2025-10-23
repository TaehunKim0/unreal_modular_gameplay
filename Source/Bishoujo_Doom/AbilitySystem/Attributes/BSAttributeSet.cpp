// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/BSAttributeSet.h"

#include "AbilitySystem/BSAbilitySystemComponent.h"

UBSAbilitySystemComponent* UBSAttributeSet::GetBSAbilitySystemComponent() const
{
	return Cast<UBSAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}

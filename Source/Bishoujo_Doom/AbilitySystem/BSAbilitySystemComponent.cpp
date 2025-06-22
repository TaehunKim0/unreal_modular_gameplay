// Fill out your copyright notice in the Description page of Project Settings.


#include "BSAbilitySystemComponent.h"
#include "BSLogChannels.h"

UBSAbilitySystemComponent::UBSAbilitySystemComponent()
{
}

void UBSAbilitySystemComponent::OnRegister()
{
	Super::OnRegister();

	UE_LOG(LogBS, Log, TEXT("UBSAbilitySystemComponent::OnRegister"));
}

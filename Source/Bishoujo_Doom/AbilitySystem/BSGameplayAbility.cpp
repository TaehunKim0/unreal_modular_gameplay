// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/BSGameplayAbility.h"

#include "Character/BSCharacter.h"

ABSCharacter* UBSGameplayAbility::GetBSCharacterFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<ABSCharacter>(CurrentActorInfo->AvatarActor.Get()) : nullptr);
}

ACharacter* UBSGameplayAbility::GetCharacterFromActorInfo() const
{
	return (CurrentActorInfo ? Cast<ACharacter>(CurrentActorInfo->AvatarActor.Get()) : nullptr);
}
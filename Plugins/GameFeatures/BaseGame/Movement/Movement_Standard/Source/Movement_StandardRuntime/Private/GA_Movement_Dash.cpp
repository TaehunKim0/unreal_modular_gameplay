// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Movement_Dash.h"

#include "Etc/BSLogChannels.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_Movement_Dash::UGA_Movement_Dash(const FObjectInitializer& ObjectInitializer)
{
}

void UGA_Movement_Dash::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ACharacter* Character = GetCharacterFromActorInfo();
	if (Character->IsLocallyControlled())
	{
		FVector ForwardDir = Character->GetActorForwardVector();
		Character->GetCharacterMovement()->Launch(ForwardDir * DashPower);

		EndAbility(Handle, ActorInfo,ActivationInfo, true, false);
	}

	UE_LOG(LogBS, Log, TEXT("UGA_Movement_Dash::ActivateAbility"));
}

void UGA_Movement_Dash::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UGA_Movement_Dash::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_Movement_Dash::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
}

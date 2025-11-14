// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Movement_Jump.h"

#include "AbilitySystemComponent.h"
#include "Etc/BSLogChannels.h"
#include "GameFramework/Character.h"
#include "Abilities/Tasks/AbilityTask_StartAbilityState.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_Movement_Jump::UGA_Movement_Jump(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UGA_Movement_Jump::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	ACharacter* Character = GetCharacterFromActorInfo();

	if (Character->IsLocallyControlled())
	{
		Character->Jump();

		const auto JumpingTask = UAbilityTask_StartAbilityState::StartAbilityState(this, "Jumping", true);
		JumpingTask->OnStateEnded.AddDynamic(this, &UGA_Movement_Jump::StopJumping);
		JumpingTask->OnStateInterrupted.AddDynamic(this, &UGA_Movement_Jump::StopJumping);
	}

	UE_LOG(LogBS, Log, TEXT("UGA_Movement_Jump::ActivateAbility"));
}

void UGA_Movement_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	UE_LOG(LogBS, Log, TEXT("UGA_Movement_Jump::EndAbility"));
}

bool UGA_Movement_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	ACharacter* Character = GetCharacterFromActorInfo();
	if (!Character || !Character->CanJump())
	{
		return false;
	}
	
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_Movement_Jump::InputReleased(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);
	
	EndAbility(Handle, ActorInfo,ActivationInfo, false, false);
}

void UGA_Movement_Jump::StopJumping()
{
	ACharacter* Character = GetCharacterFromActorInfo();
	if (Character->IsLocallyControlled())
	{
		Character->StopJumping();

		UE_LOG(LogBS, Log, TEXT("UGA_Movement_Jump::StopJumping"));
	}
}
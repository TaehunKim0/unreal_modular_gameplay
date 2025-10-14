// Fill out your copyright notice in the Description page of Project Settings.


#include "BSAbilitySystemComponent.h"

#include "BSGameplayAbility.h"
#include "BSLogChannels.h"

UBSAbilitySystemComponent::UBSAbilitySystemComponent()
{
}

void UBSAbilitySystemComponent::OnRegister()
{
	Super::OnRegister();

	UE_LOG(LogBS, Log, TEXT("UBSAbilitySystemComponent::OnRegister"));
}

void UBSAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);

	// We don't support UGameplayAbility::bReplicateInputDirectly.
	// Use replicated events instead so that the WaitInputPress ability task works.
	if (Spec.IsActive())
	{
		// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void UBSAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	if (Spec.IsActive())
	{
		// Invoke the InputReleased event. This is not replicated here. If someone is listening, they may replicate the InputReleased event to the server.
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void UBSAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	// if (HasMatchingGameplayTag(TAG_Gameplay_AbilityInputBlocked))
	// {
	// 	ClearAbilityInput();
	// 	return;
	// }
	
	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();
	
	// //
	// // Process all abilities that activate when the input is held.
	// //
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		if (const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability && !AbilitySpec->IsActive())
			{
				const UBSGameplayAbility* BSAbilityCDO = CastChecked<UBSGameplayAbility>(AbilitySpec->Ability);
	
				if (BSAbilityCDO->GetActivationPolicy() == EBSAbilityActivationPolicy::WhileInputActive)
				{
					AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
				}
			}
		}
	}
	//
	// //
	// // Process all abilities that had their input pressed this frame.
	// //
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;
	
				if (AbilitySpec->IsActive())
				{
					// Ability is active, so pass along the input event.
					AbilitySpecInputPressed(*AbilitySpec);
				}
				else
				{
					const UBSGameplayAbility* BSAbilityCDO = CastChecked<UBSGameplayAbility>(AbilitySpec->Ability);
	
					if (BSAbilityCDO->GetActivationPolicy() == EBSAbilityActivationPolicy::InputTriggeredOnce)
					{
						AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
					}
				}
			}
		}
	}

	//
	// Try to activate all the abilities that are from presses and holds.
	// We do it all at once so that held inputs don't activate the ability
	// and then also send an input event to the ability because of the press.
	//
	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		if (const FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(AbilitySpecHandle))
		{
			const FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
			if (!ActorInfo->AvatarActor.IsValid())
			{
				UE_LOG(LogBS, Error, TEXT("Invalid AvatarActor: %s"), *AbilitySpecHandle.ToString());
			}
			TryActivateAbility(AbilitySpecHandle);
		}
		else
		{
			UE_LOG(LogBS, Error, TEXT("Invalid AbilitySpecHandle: %s"), *AbilitySpecHandle.ToString());
		}
	}

	//
	// Process all abilities that had their input released this frame.
	//
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false;
	
				if (AbilitySpec->IsActive())
				{
					// Ability is active, so pass along the input event.
					AbilitySpecInputReleased(*AbilitySpec);
				}
			}
		}
	}

	//
	// Clear the cached ability handles.
	//
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void UBSAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)))
			{
				InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
		
				UE_LOG(LogBS, Log, TEXT("UBSAbilitySystemComponent::AbilityInputTagPressed Tag : %s"), *InputTag.ToString());
			}
		}
	}
}

void UBSAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)))
			{
				InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.Remove(AbilitySpec.Handle);
		
				UE_LOG(LogBS, Log, TEXT("UBSAbilitySystemComponent::AbilityInputTagReleased Tag : %s"), *InputTag.ToString());
			}
		}
	}
}

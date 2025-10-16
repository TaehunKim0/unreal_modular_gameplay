// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFeatures/GameFeatureAction_AddAbilities.h"

#include "AbilitySystemComponent.h"
#include "BSLogChannels.h"
#include "EngineUtils.h"
#include "GameFeaturesSubsystem.h"
#include "AbilitySystem/BSAbilitySet.h"
#include "Character/BSCharacter.h"

void UGameFeatureAction_AddAbilities::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	Super::OnGameFeatureActivating(Context);

	for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
	{
		if (Context.ShouldApplyToWorldContext(WorldContext))
		{
			if (UWorld* World = WorldContext.World())
			{
				for (TActorIterator<APawn> It(World); It; ++It)
				{
					APawn* Pawn = *It;
					if (!IsValid(Pawn))
					{
						continue;
					}

					if (!Pawn->IsLocallyControlled())
					{
						continue;
					}

					for (const auto& AbilitiyEntry : AbilitiesArray)
					{
						if (UClass* TargetActorClass = AbilitiyEntry.TargetActorClass.LoadSynchronous())
						{
							if (Pawn->IsA(TargetActorClass))
							{
								AddActorAbilities(Pawn, AbilitiyEntry);
							}
						}
						else
						{
							UE_LOG(LogBS, Warning, TEXT("UGameFeatureAction_AddAbilities::Abilities 적용 중 타겟 액터 클래스 로드 실패: %s"), *AbilitiyEntry.TargetActorClass.ToString());
						}
					}
				}
			}
		}
	}
}

void UGameFeatureAction_AddAbilities::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);

	while (!AddedAbilitiesMap.IsEmpty())
	{
		auto It = AddedAbilitiesMap.CreateIterator();
        
		if (IsValid(It->Key) && It->Key->IsValidLowLevel())
		{
			ABSCharacter* BSCharacter = Cast<ABSCharacter>(It->Key);
			if (BSCharacter && BSCharacter->GetAbilitySystemComponent())
			{
				RemoveActorAbilities(BSCharacter);
				continue;
			}
		}
		
		It.RemoveCurrent();
	}
    
	AddedAbilitiesMap.Reset();
}

void UGameFeatureAction_AddAbilities::AddActorAbilities(AActor* InActor, const FGameFeatureAbilitiesEntry& InAbilitiesEntry)
{
	check(InActor);
	if (!InActor->HasAuthority())
	{
		return;
	}

	ABSCharacter* BSCharacter = Cast<ABSCharacter>(InActor);
	if (!BSCharacter)
	{
		return;
	}

	UAbilitySystemComponent* ASC = BSCharacter->GetAbilitySystemComponent();
	if (!IsValid(ASC))
	{
		UE_LOG(LogBS, Error, TEXT("UGameFeatureAction_AddAbilities::액터 %s에게 ASC 가 없음"), *InActor->GetName());
		return;
	}

	TArray<FGameplayAbilitySpecHandle>& AbilitySpecHandles = AddedAbilitiesMap.FindOrAdd(InActor);
	for (auto AbilitySet : InAbilitiesEntry.GrantAbilitySets)
	{
		const UBSAbilitySet* LoadAbilitySet = AbilitySet.LoadSynchronous();
		if (IsValid(LoadAbilitySet))
		{
			for (auto GrantAbility : LoadAbilitySet->GrantedGameplayAbilitiesWithInputTag)
			{
				FGameplayAbilitySpec Spec(GrantAbility.Ability, GrantAbility.AbilityLevel, INDEX_NONE, InActor);
				Spec.GetDynamicSpecSourceTags().AddTag(GrantAbility.InputTag);
				
				FGameplayAbilitySpecHandle AbilityHandle = ASC->GiveAbility(Spec);

				if (AbilityHandle.IsValid())
				{
					AbilitySpecHandles.Add(AbilityHandle);
					UE_LOG(LogBS, Warning, TEXT("UGameFeatureAction_AddAbilities::액터 %s에 어빌리티 %s 부여 성공"), *InActor->GetName(), *GrantAbility.Ability->GetName());
				}
				else
				{
					UE_LOG(LogBS, Error, TEXT("UGameFeatureAction_AddAbilities::액터 %s에 어빌리티 %s 부여 실패"), *InActor->GetName(), *GrantAbility.Ability->GetName());
				}
			}
		}
	}
}

void UGameFeatureAction_AddAbilities::RemoveActorAbilities(AActor* InActor)
{
	check(InActor);
	if (!InActor->HasAuthority())
	{
		return;
	}

	ABSCharacter* BSCharacter = Cast<ABSCharacter>(InActor);
	if (!BSCharacter)
	{
		return;
	}

	UAbilitySystemComponent* ASC = BSCharacter->GetAbilitySystemComponent();
	check(ASC);
	if (!ASC)
	{
		UE_LOG(LogBS, Error, TEXT("RemoveActorAbilities::액터 %s에게 ASC 가 없음"), *InActor->GetName());
		return;
	}
	
	if (const auto ActorAbilitieHandles = AddedAbilitiesMap.Find(InActor))
	{
		for (const FGameplayAbilitySpecHandle AbilityHandle : *ActorAbilitieHandles)
		{
			if (AbilityHandle.IsValid())
			{
				ASC->ClearAbility(AbilityHandle);
				UE_LOG(LogBS, Warning, TEXT("UGameFeatureAction_AddAbilities::액터 %s 의 어빌리티 핸들 %s 제거 완료"), *InActor->GetName(), *AbilityHandle.ToString());
			}
		}

		AddedAbilitiesMap.Remove(InActor);
	}
	
}

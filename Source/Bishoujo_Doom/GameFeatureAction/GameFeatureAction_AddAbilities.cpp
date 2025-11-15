// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFeatureAction/GameFeatureAction_AddAbilities.h"

#include "AbilitySystemComponent.h"
#include "Etc/BSLogChannels.h"
#include "EngineUtils.h"
#include "GameFeaturesSubsystem.h"
#include "AbilitySystem/Abilities/BSAbilitySet.h"
#include "Character/BSCharacter.h"

UGameFeatureAction_AddAbilities::UGameFeatureAction_AddAbilities(const FObjectInitializer& ObjectInitializer)
{
	UE_LOG(LogBS, Warning, TEXT("UGameFeatureAction_AddAbilities 생성"));
}

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

	FAbilitiesHandle& AbilitiesHandle = AddedAbilitiesMap.FindOrAdd(InActor);
	
	for (auto AbilitySet : InAbilitiesEntry.GrantAbilitySets)
	{
		const UBSAbilitySet* LoadAbilitySet = AbilitySet.LoadSynchronous();
		if (IsValid(LoadAbilitySet))
		{
			// Ability
			for (auto AbilityToGrant : LoadAbilitySet->GrantAbilitiesWithInputTag)
			{
				FGameplayAbilitySpec Spec(AbilityToGrant.Ability, AbilityToGrant.AbilityLevel, INDEX_NONE, InActor);
				Spec.GetDynamicSpecSourceTags().AddTag(AbilityToGrant.InputTag);

				FGameplayAbilitySpecHandle AbilityHandle = ASC->GiveAbility(Spec);

				if (AbilityHandle.IsValid())
				{
					AbilitiesHandle.AbilitySpecArray.Add(AbilityHandle);
					UE_LOG(LogBS, Warning, TEXT("UGameFeatureAction_AddAbilities::액터 %s에 어빌리티 %s 부여 성공"), *InActor->GetName(), *AbilityToGrant.Ability->GetName());
				}
				else
				{
					UE_LOG(LogBS, Error, TEXT("UGameFeatureAction_AddAbilities::액터 %s에 어빌리티 %s 부여 실패"), *InActor->GetName(), *AbilityToGrant.Ability->GetName());
				}
			}

			// GameplayEffect
			for (auto GameplayEffectToGrant : LoadAbilitySet->GrantGameplayEffects)
			{
				const UGameplayEffect* GameplayEffect = GameplayEffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
				const FActiveGameplayEffectHandle GameplayEffectHandle = ASC->ApplyGameplayEffectToSelf(GameplayEffect, GameplayEffectToGrant.EffectLevel, ASC->MakeEffectContext());

				if (GameplayEffectHandle.IsValid())
				{
					AbilitiesHandle.GameplayEffectHandleArray.Add(GameplayEffectHandle);
					UE_LOG(LogBS, Warning, TEXT("UGameFeatureAction_AddAbilities::액터 %s에 게임플레이 이펙트 %s 부여 성공"), *InActor->GetName(), *GameplayEffectToGrant.GameplayEffect->GetName());
				}
				else
				{
					UE_LOG(LogBS, Error, TEXT("UGameFeatureAction_AddAbilities::액터 %s에 게임플레이 이펙트 %s 부여 실패"), *InActor->GetName(), *GameplayEffectToGrant.GameplayEffect->GetName());
				}
			}

			// Attribute
			for (auto AttributeToGrant : LoadAbilitySet->GrantAttributeSets)
			{
				UAttributeSet* NewAttributeSet = NewObject<UAttributeSet>(ASC, AttributeToGrant.AttributeSet);
				if (NewAttributeSet)
				{
					const auto ResultAttributeSet = ASC->AddAttributeSetSubobject(NewAttributeSet);
					if (ResultAttributeSet)
					{
						AbilitiesHandle.AttributeSetArray.Add(NewAttributeSet);
						UE_LOG(LogBS, Warning, TEXT("UGameFeatureAction_AddAbilities::액터 %s에 게임플레이 어트리뷰트 %s 부여 성공"), *InActor->GetName(), *AttributeToGrant.AttributeSet->GetName());
					}
					else
					{
						UE_LOG(LogBS, Error, TEXT("UGameFeatureAction_AddAbilities::액터 %s에 게임플레이 어트리뷰트 %s 부여 실패"), *InActor->GetName(), *AttributeToGrant.AttributeSet->GetName());
					}
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
	
	if (const auto AbilitiesHandle = AddedAbilitiesMap.Find(InActor))
	{
		// Ability
		for (const FGameplayAbilitySpecHandle& AbilityHandle : AbilitiesHandle->AbilitySpecArray)
		{
			if (AbilityHandle.IsValid())
			{
				ASC->ClearAbility(AbilityHandle);
				UE_LOG(LogBS, Warning, TEXT("UGameFeatureAction_AddAbilities::액터 %s 의 어빌리티 핸들 %s 제거 완료"), *InActor->GetName(), *AbilityHandle.ToString());
			}
		}

		// GameplayEffect
		for (const FActiveGameplayEffectHandle& EffectHandle : AbilitiesHandle->GameplayEffectHandleArray)
		{
			if (EffectHandle.IsValid())
			{
				ASC->RemoveActiveGameplayEffect(EffectHandle);
				UE_LOG(LogBS, Warning, TEXT("UGameFeatureAction_AddAbilities::액터 %s 의 게임플레이 이펙트 %s 제거 완료"), *InActor->GetName(), *EffectHandle.ToString());
			}
		}

		// Attribute
		for (UAttributeSet* AttributeSet : AbilitiesHandle->AttributeSetArray)
		{
			if (AttributeSet)
			{
				ASC->RemoveSpawnedAttribute(AttributeSet);
				UE_LOG(LogBS, Warning, TEXT("UGameFeatureAction_AddAbilities::액터 %s 의 어트리뷰트 %s 제거 완료"), *InActor->GetName(), *AttributeSet->GetFName().ToString());
			}
		}

		AbilitiesHandle->AbilitySpecArray.Reset();
		AbilitiesHandle->GameplayEffectHandleArray.Reset();
		AbilitiesHandle->AttributeSetArray.Reset();
		
		AddedAbilitiesMap.Remove(InActor);
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/BSHealthAttributeSet.h"

#include "Etc/BSLogChannels.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/BSAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

UBSHealthAttributeSet::UBSHealthAttributeSet()
	: Health(100.f)
	, MaxHealth(100.f)
{
	HealthBeforeAttributeChange = 0.0f;
	MaxHealthBeforeAttributeChange = 0.0f;
}

void UBSHealthAttributeSet::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UBSHealthAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UBSHealthAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
}

bool UBSHealthAttributeSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	// 예: 무적 상태면 데미지 거부
	// const FGameplayTagContainer* SourceTags = Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
	// if (SourceTags && SourceTags->HasTag(FGameplayTag::RequestGameplayTag("State.Invincible")))
	// {
	//     return false; // Effect 적용 거부
	// }

	//사망한 캐릭터는 더 이상 데미지나 힐을 받지 않음
	if (Health.GetCurrentValue() <= 0.0f)
	{
		if (Data.EvaluatedData.Attribute == GetDamageAttribute() || 
			Data.EvaluatedData.Attribute == GetHealingAttribute())
		{
			return false;
		}
	}

	return true;
}

void UBSHealthAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// Health는 0과 MaxHealth 사이로 제한
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	// MaxHealth는 최소 1 이상
	else if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}

	HealthBeforeAttributeChange = GetHealth();
	MaxHealthBeforeAttributeChange = GetMaxHealth();
}

void UBSHealthAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
	AActor* Instigator = EffectContext.GetOriginalInstigator();
	AActor* Causer = EffectContext.GetEffectCauser();

	// Damage
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth() - GetDamage(), 0.0f, GetMaxHealth()));
		SetDamage(0.0f);
	}
	// Healing
	else if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth() + GetHealing(), 0.0f, GetMaxHealth()));
		SetHealing(0.0f); 
	}
	// Set Health
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}
	// Set MaxHealth
	else if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		SetMaxHealth(FMath::Clamp(GetMaxHealth(), 0.0f, GetMaxHealth()));
	}

	// Broadcasts
	if (GetHealth() != HealthBeforeAttributeChange)
	{
		OnHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, HealthBeforeAttributeChange, GetHealth());
	}

	if (GetMaxHealth() != MaxHealthBeforeAttributeChange)
	{
		OnMaxHealthChanged.Broadcast(Instigator, Causer, &Data.EffectSpec, Data.EvaluatedData.Magnitude, MaxHealthBeforeAttributeChange, GetMaxHealth());
	}
}

void UBSHealthAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		// MaxHealth가 줄어들어서 현재 Health가 초과하는 경우
		if (GetHealth() > NewValue)
		{
			UBSAbilitySystemComponent* BSASC = GetBSAbilitySystemComponent();
			check(BSASC);

			// Health를 새로운 MaxHealth로 강제 설정
			BSASC->ApplyModToAttribute(GetHealthAttribute(), 
										 EGameplayModOp::Override,
										 NewValue);

			// SetHealth 하면 무한 재귀
		}
	}
}

void UBSHealthAttributeSet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	UE_LOG(LogBS, Warning, TEXT("UBSHealthAttributeSet::OnRep_Health %f -> %f"), OldValue.GetCurrentValue(), GetHealth());
}

void UBSHealthAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	UE_LOG(LogBS, Warning, TEXT("UBSHealthAttributeSet::OnRep_MaxHealth %f -> %f"), OldValue.GetCurrentValue(), GetMaxHealth());
}
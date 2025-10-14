// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFeatures/GameFeatureAction_AddAttributes.h"

#include "AbilitySystemComponent.h"
#include "BSLogChannels.h"
#include "EngineUtils.h"
#include "GameFeaturesSubsystem.h"

void UGameFeatureAction_AddAttributes::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
    Super::OnGameFeatureActivating(Context);

    for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
    {
        if (Context.ShouldApplyToWorldContext(WorldContext))
        {
            if (UWorld* World = WorldContext.World())
            {
                // 모든 Pawn을 순회 (플레이어와 적 Pawn 포함)
                for (TActorIterator<APawn> It(World); It; ++It)
                {
                    APawn* Pawn = *It;
                    if (!IsValid(Pawn))
                    {
                        continue;
                    }

                    // AttributesList의 각 엔트리에 대해 처리
                    for (const auto& AttributesEntry : AttributesArray)
                    {
                        if (UClass* TargetActorClass = AttributesEntry.TargetActorClass.LoadSynchronous())
                        {
                            if (Pawn->IsA(TargetActorClass))
                            {
                                AddActorAttributes(Pawn, AttributesEntry);
                            }
                        }
                        else
                        {
                            UE_LOG(LogBS, Warning, TEXT("UGameFeatureAction_AddAttributes::Attribute 적용 중 타겟 액터 클래스 로드 실패: %s"), *AttributesEntry.TargetActorClass.ToString());
                        }
                    }
                }
            }
        }
    }
}

void UGameFeatureAction_AddAttributes::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
    Super::OnGameFeatureDeactivating(Context);

    while (!AddedAttributesMap.IsEmpty())
    {
        auto It = AddedAttributesMap.CreateIterator();

        if (IsValid(It->Key))
        {
            if (AActor* Actor = It->Key)
            {
                RemoveActorAttributes(Actor);
            }
        }
    }
    
    AddedAttributesMap.Reset();
}

void UGameFeatureAction_AddAttributes::AddActorAttributes(AActor* InActor, const FGameFeatureAttributesEntry& InAttributesEntry)
{
    check(InActor);
    if (!InActor->HasAuthority())
    {
        return;
    }
    
    UAbilitySystemComponent* ASC = InActor->FindComponentByClass<UAbilitySystemComponent>();
    if (!ASC)
    {
        UE_LOG(LogBS, Error, TEXT("UGameFeatureAction_AddAttributes::액터에게 ASC 가 없습니다."));
        return;
    }
    
    TArray<UAttributeSet*>& ActorAttributes = AddedAttributesMap.FindOrAdd(InActor);

    for (const TSoftClassPtr<UAttributeSet>& AttributeSetClass : InAttributesEntry.AttributeSets)
    {
        if (UClass* LoadedAttributeSetClass = AttributeSetClass.LoadSynchronous())
        {
            UAttributeSet* NewAttributeSet = NewObject<UAttributeSet>(ASC, LoadedAttributeSetClass);
            if (NewAttributeSet)
            {
                ASC->AddAttributeSetSubobject(NewAttributeSet);
                ActorAttributes.Add(NewAttributeSet);
            }
        }
    }
}

void UGameFeatureAction_AddAttributes::RemoveActorAttributes(const AActor* InActor)
{
    check(InActor);
    if (!InActor->HasAuthority())
    {
        return;
    }

    if (UAbilitySystemComponent* AbilitySystemComponent = InActor->FindComponentByClass<UAbilitySystemComponent>())
    {
        if (TArray<UAttributeSet*>* ActorAttributes = AddedAttributesMap.Find(InActor))
        {
            for (UAttributeSet* AttributeSet : *ActorAttributes)
            {
                if (AttributeSet)
                {
                    AbilitySystemComponent->RemoveSpawnedAttribute(AttributeSet);
                }
            }
            
            AddedAttributesMap.Remove(InActor);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("UGameFeatureAction_AddAttributes::Attribute Set 제거 시 액터 %s에 AbilitySystemComponent가 없습니다."), *InActor->GetName());
    }
}
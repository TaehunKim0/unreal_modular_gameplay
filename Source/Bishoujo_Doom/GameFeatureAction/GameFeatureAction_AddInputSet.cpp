// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFeatureAction/GameFeatureAction_AddInputSet.h"

#include "EngineUtils.h"
#include "EnhancedPlayerInput.h"
#include "GameFeaturesSubsystem.h"
#include "Character/Component/BSPawnInputComponent.h"
#include "Elements/Columns/TypedElementRevisionControlColumns.h"
#include "Input/BSInputComponent.h"

void UGameFeatureAction_AddInputSet::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
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
                    
					for (const auto& InputSetEntry : InputSetArray)
					{
						if (UClass* TargetActorClass = InputSetEntry.TargetActorClass.LoadSynchronous())
						{
							if (Pawn->IsA(TargetActorClass))
							{
								AddActorInputSet(Pawn, InputSetEntry);
							}
						}
						else
						{
							UE_LOG(LogBS, Warning, TEXT("UGameFeatureAction_AddInputSet:: 적용 중 타겟 액터 클래스 로드 실패: %s"), *InputSetEntry.TargetActorClass.ToString());
						}
					}
				}
			}
		}
	}
}

void UGameFeatureAction_AddInputSet::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);

	while (!AddedInputSetMap.IsEmpty())
	{
		auto It = AddedInputSetMap.CreateIterator();
		auto Pawn = It->Key;
		if (IsValid(Pawn) && Pawn->IsValidLowLevel())
		{
			if (CastChecked<AActor>(Pawn))
			{
				RemoveActorInputSet(Pawn);
				continue;
			}
		}

		It.RemoveCurrent();
	}
    
	AddedInputSetMap.Reset();
}

void UGameFeatureAction_AddInputSet::AddActorInputSet(APawn* InPawn, const FGameFeatureInputSetEntry& InInputSetEntry)
{
	for (const auto& InputSetEntry : InputSetArray)
	{
		const UBSInputSet* InputSet = InputSetEntry.GrantInputSet.LoadSynchronous();
		if (!InputSet)
		{
			return;
		}
		
		const auto DefaultCharacterComp = InPawn->FindComponentByClass<UBSPawnInputComponent>();
		if (!DefaultCharacterComp)
		{
			UE_LOG(LogBS, Error, TEXT("UGameFeatureAction_AddInputSet::DefaultCharacterComp Not Found!"));
			continue;
		}
		
		TArray<uint32> BindHandles;
		DefaultCharacterComp->AddAdditionalBindAction(InputSet, BindHandles);
		UE_LOG(LogBS, Warning, TEXT("UGameFeatureAction_AddInputSet:: InputSet %s 추가 완료"), *InputSet->GetName());
		
		TArray<uint32>& ActorInputSetArray = AddedInputSetMap.FindOrAdd(InPawn);
		ActorInputSetArray.Append(BindHandles);
	}
}

void UGameFeatureAction_AddInputSet::RemoveActorInputSet(APawn* InPawn)
{
	auto RemoveInputSetHandle = AddedInputSetMap.FindOrAdd(InPawn);
	const auto DefaultCharacterComp = InPawn->FindComponentByClass<UBSPawnInputComponent>();
	if (!DefaultCharacterComp)
	{
		UE_LOG(LogBS, Error, TEXT("UGameFeatureAction_AddInputSet::DefaultCharacterComp Not Found!"));
		return;
	}

	for (const int32 Handle : RemoveInputSetHandle)
	{
		DefaultCharacterComp->RemoveAdditionalBindAction(Handle);
		UE_LOG(LogBS, Warning, TEXT("UGameFeatureAction_AddInputSet::액터 %s 의 InputSetHandle %d 제거 완료"), *InPawn->GetName(), Handle);
	}
	
	AddedInputSetMap.Remove(InPawn);
}
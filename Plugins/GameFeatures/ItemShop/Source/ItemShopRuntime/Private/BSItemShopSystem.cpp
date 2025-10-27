// Fill out your copyright notice in the Description page of Project Settings.


#include "BSItemShopSystem.h"

#include "BSLogChannels.h"
#include "DataRegistrySubsystem.h"

void UBSItemShopSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	OnAcquireShopItemsCompleteDelegate.BindUObject(this, &UBSItemShopSystem::OnItemDataLoaded);
}

void UBSItemShopSystem::LoadShopItems(FGameplayTagContainer FilterTags, const FDataRegistryItemAcquiredCallback& InCallback)
{
	UDataRegistrySubsystem* DRSubsystem = UDataRegistrySubsystem::Get();
	if (!DRSubsystem)
	{
		return;
	}

	auto ResultRegistry = DRSubsystem->GetRegistryForType(FName("ShopItems"));
	if (!ResultRegistry)
	{
		
		return;
	}

	// Data Registry에서 모든 아이템 ID 가져오기
	// GameFeatureAction 으로 Data Registry 추가하는 것은 Registering 단계에서 하기 때문에 밑에서 레지스트리가 로드 되지 않는 경우 X
	// void UGameFeatureAction_DataRegistry::OnGameFeatureRegistering()
	
	TArray<FDataRegistryId> AllItemIds;
	DRSubsystem->GetPossibleDataRegistryIdList(FName("ShopItems"), AllItemIds);

	// 비동기로 각 아이템 로드
	int32 TotalItems = AllItemIds.Num();
	TSharedPtr<int32> LoadedCount = MakeShared<int32>(0);

	for (const FDataRegistryId& ItemId : AllItemIds)
	{
		DRSubsystem->AcquireItem(ItemId, 
			FDataRegistryItemAcquiredCallback::CreateLambda(
				[this, FilterTags, LoadedCount, InCallback, TotalItems]
				(const FDataRegistryAcquireResult& Result)
				{
					const FItemDataRow* ItemData = Result.GetItem<FItemDataRow>();
					if (ItemData)
					{
						// 필터 태그 체크
						if (FilterTags.IsEmpty() || ItemData->ItemTag.HasAny(FilterTags))
						{
							FPrimaryAssetId AssetId("Item", ItemData->PrimaryAssetName);
							if (!CachedItemIds.Contains(AssetId))
								CachedItemIds.Add(AssetId);
						}
					}
                    
					(*LoadedCount)++;
                    
					// 모두 로드 완료
					if (*LoadedCount >= TotalItems)
					{
						InCallback.Execute(Result);
						OnAcquireShopItemsCompleteDelegate.Execute();
					}
				}
			)
		);
	}
}

bool UBSItemShopSystem::GetCachedItemData(FPrimaryAssetId ItemId, FItemDataRow& OutData)
{
	// 캐시에 없으면 즉시 시도
	UDataRegistrySubsystem* DRSubsystem = UDataRegistrySubsystem::Get();
	TArray<UDataRegistry*> OutRegistries;
	TArray<FName> OutNames;
	DRSubsystem->GetAllRegistries(OutRegistries);

	for (auto Reg : OutRegistries)
	{
		Reg->GetItemNames(OutNames);
	}

	for (auto Name : OutNames)
	{
		UE_LOG(LogBS, Warning, TEXT("Loaded Item Names : %s"), *Name.ToString());
	}
	
	if (DRSubsystem)
	{
		FDataRegistryId RegistryId(FName("ShopItems"), ItemId.PrimaryAssetName);
		const FItemDataRow* Data = DRSubsystem->GetCachedItem<FItemDataRow>(RegistryId);
		if (Data)
		{
			OutData = *Data;
			return true;
		}
	}
    
	return false;
}

bool UBSItemShopSystem::PurchaseItem(FPrimaryAssetId ItemId)
{
	return false;
}

const TArray<FItemDataRow> UBSItemShopSystem::GetAllItems()
{
	TArray<FItemDataRow> AllItemData;
	TArray<FPrimaryAssetId> RemoveItemId;

	/*
	*Add_DataRegistry 를 먼저 Deactive하고
	Add_DataRegistrySource 를 Deactve  해서 

	DataRegistry 에 등록된 소스들이 먼저 다 해제 됐나봐
	 */
	for (FPrimaryAssetId& ItemId : CachedItemIds)
	{
		FItemDataRow OutItemData;
		auto bResult = GetCachedItemData(ItemId, OutItemData);
		if (!bResult)
		{
			RemoveItemId.Add(ItemId);
			continue;
		}
		
		AllItemData.Add(OutItemData);
	}

	for (auto RemoveId : RemoveItemId)
	{
		CachedItemIds.Remove(RemoveId);
	}

	return AllItemData;
}

void UBSItemShopSystem::OnItemDataLoaded()
{
	
}

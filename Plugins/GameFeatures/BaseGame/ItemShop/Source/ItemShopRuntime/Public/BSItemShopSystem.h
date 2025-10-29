// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataRegistryTypes.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BSItemShopSystem.generated.h"

struct FDataRegistryId;

USTRUCT(BlueprintType)
struct FItemDataRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName PrimaryAssetName;
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString Description;
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Price;
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> Icon;
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer ItemTag;
};

/**
 * 
 */
UCLASS()
class ITEMSHOPRUNTIME_API UBSItemShopSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
public:
	void LoadShopItems(FGameplayTagContainer FilterTags, const FDataRegistryItemAcquiredCallback& InCallback);
	bool GetCachedItemData(FPrimaryAssetId ItemId, FItemDataRow& OutData);
	bool PurchaseItem(FPrimaryAssetId ItemId);
	const TArray<FItemDataRow> GetAllItems();

private:
	void OnItemDataLoaded();
	
private:

	TArray<FPrimaryAssetId> CachedItemIds;
	
	FSimpleDelegate OnAcquireShopItemsCompleteDelegate;
};

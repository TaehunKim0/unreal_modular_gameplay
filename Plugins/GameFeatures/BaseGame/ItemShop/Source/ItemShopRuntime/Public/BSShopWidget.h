// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataRegistryTypes.h"
#include "Blueprint/UserWidget.h"
#include "BSShopWidget.generated.h"

class UBSShopItemWidget;
class UScrollBox;
/**
 * 
 */
UCLASS()
class ITEMSHOPRUNTIME_API UBSShopWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
	void OnShopItemsLoaded(const FDataRegistryAcquireResult& Result);

protected:
	UPROPERTY(meta=(BindWidget))
	UScrollBox* ItemListScrollBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UBSShopItemWidget> ShopItemWidgetClass;
	
	FDataRegistryItemAcquiredCallback Callback;

};


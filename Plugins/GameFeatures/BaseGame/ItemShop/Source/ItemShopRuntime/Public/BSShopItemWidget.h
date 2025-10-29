// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BSShopItemWidget.generated.h"

struct FItemDataRow;
class UImage;
class UTextBlock;
/**
 * 
 */
UCLASS()
class ITEMSHOPRUNTIME_API UBSShopItemWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetItemData(const FItemDataRow& InItemData);
	
protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UImage* Icon;
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UTextBlock* ItemName;
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UTextBlock* ItemPrice;
};
// Fill out your copyright notice in the Description page of Project Settings.


#include "BSShopItemWidget.h"

#include "BSItemShopSystem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UBSShopItemWidget::SetItemData(const FItemDataRow& InItemData)
{
	Icon->SetBrushFromTexture(InItemData.Icon.LoadSynchronous());
	ItemName->SetText(FText::FromString(InItemData.DisplayName));

	FString FPrice; FPrice.AppendInt(InItemData.Price);
	ItemPrice->SetText(FText::FromString(FPrice));
}

void UBSShopItemWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

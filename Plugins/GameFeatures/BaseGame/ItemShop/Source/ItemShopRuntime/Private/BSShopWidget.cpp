// Fill out your copyright notice in the Description page of Project Settings.


#include "BSShopWidget.h"

#include "BSItemShopSystem.h"
#include "Etc/BSLogChannels.h"
#include "BSShopItemWidget.h"
#include "Components/ScrollBox.h"

void UBSShopWidget::NativeConstruct()
{
	Super::NativeConstruct();

	auto ShopSubSystem = GetGameInstance()->GetSubsystem<UBSItemShopSystem>();
	ensure(ShopSubSystem);

	FGameplayTagContainer FilterTags;
	FilterTags.AddTag(FGameplayTag::RequestGameplayTag("Item"));

	Callback.BindUObject(this, &UBSShopWidget::OnShopItemsLoaded);
	ShopSubSystem->LoadShopItems(FilterTags, Callback);
}

void UBSShopWidget::OnShopItemsLoaded(const FDataRegistryAcquireResult& Result)
{
	auto ShopSubSystem = GetGameInstance()->GetSubsystem<UBSItemShopSystem>();
	ensure(ShopSubSystem);
	
	for (const auto& ItemData : ShopSubSystem->GetAllItems())
	{
		// ShopItemWidget 생성
		UBSShopItemWidget* ItemWidget = CreateWidget<UBSShopItemWidget>(this, ShopItemWidgetClass);
		if (ItemWidget)
		{
			ItemWidget->SetItemData(ItemData);
			if (!ItemListScrollBox->HasChild(ItemWidget))
				ItemListScrollBox->AddChild(ItemWidget);
			
			//ItemWidget->OnClicked.AddDynamic(this, &UShopWidget::OnItemButtonClicked);
		}
		else
		{
			UE_LOG(LogBS, Error, TEXT("UBSShopWidget::Create ShopItemWidget Failed"));
		}
	}
}

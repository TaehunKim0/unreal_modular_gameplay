// Fill out your copyright notice in the Description page of Project Settings.


#include "BSDebugWidget.h"

#include "BSDebugBoxElementWidget.h"
#include "BSLogChannels.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"

void UBSDebugWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ShowDebugMessage("TestValue","1");
}

void UBSDebugWidget::ShowDebugMessage(const FString& InVariableName, const FString& InDesc)
{
	UBSDebugBoxElementWidget* DebugBoxElement = nullptr;
	
	if (DebugBoxElementWidgetArray.Contains(InVariableName))
	{
		const auto TempDebugBoxElement = DebugBoxElementWidgetArray[InVariableName];
		DebugBoxElement = Cast<UBSDebugBoxElementWidget>(TempDebugBoxElement);
	}
	else
	{
		DebugBoxElementWidgetArray.Emplace(InVariableName, CreateWidget(GetOwningPlayer(), DebugBoxElementWidget));
		DebugBoxElement = Cast<UBSDebugBoxElementWidget>(DebugBoxElementWidgetArray[InVariableName]);
		VerticalBox->AddChild(DebugBoxElement);
	}

	if (!DebugBoxElement)
	{
		UE_LOG(LogBS, Warning, TEXT("DebugBoxElement is null"));
	}
	else
	{
		DebugBoxElement->VariableNameTextBlock->SetText(FText::FromString(InVariableName));
		DebugBoxElement->DescTextBlock->SetText(FText::FromString(InDesc));
	}
}
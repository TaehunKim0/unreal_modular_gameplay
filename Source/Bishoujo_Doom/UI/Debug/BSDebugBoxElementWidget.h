// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BSDebugBoxElementWidget.generated.h"

class UTextBlock;
class UHorizontalBox;
/**
 * 
 */
UCLASS()
class BISHOUJO_DOOM_API UBSDebugBoxElementWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	UTextBlock* VariableNameTextBlock;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* DescTextBlock;
};

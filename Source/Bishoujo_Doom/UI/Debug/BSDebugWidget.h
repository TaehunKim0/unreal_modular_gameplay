// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BSDebugWidget.generated.h"

/**
 * 
 */

class UVerticalBox;
class UBSDebugBoxElementWidget;

UCLASS()
class BISHOUJO_DOOM_API UBSDebugWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

public:
	void ShowDebugMessage(const FString& InVariableName, const FString& InDesc);
	
protected:
	UPROPERTY(meta=(BindWidget))
	UVerticalBox* VerticalBox;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UBSDebugBoxElementWidget> DebugBoxElementWidget;

	UPROPERTY()
	TMap<FString, UUserWidget*> DebugBoxElementWidgetArray;
};
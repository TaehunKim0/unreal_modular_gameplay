// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "BSCharacterSelectionWidget.generated.h"

class UBSCharacterDefinition;
/**
 * 
 */
UCLASS()
class BISHOUJO_DOOM_API UBSCharacterSelectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void OnSpidyButtonPressed();

	UFUNCTION()
	void OnDefaultButtonPressed();

	void SetCharacterDefinition(FGameplayTag InTag);
	
protected:
	UPROPERTY(meta=(BindWidget))
	UButton* SpidyButton;

	UPROPERTY(meta=(BindWidget))
	UButton* DefaultButton;
};
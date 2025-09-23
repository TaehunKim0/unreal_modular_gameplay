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

private:
	UFUNCTION()
	void OnEllieButtonPressed();

	UFUNCTION()
	void OnJoelButtonPressed();

	void SetCharacterDefinition(FGameplayTag InTag);
	
protected:
	UPROPERTY(meta=(BindWidget))
	UButton* EllieButton;

	UPROPERTY(meta=(BindWidget))
	UButton* JoelButton;
};
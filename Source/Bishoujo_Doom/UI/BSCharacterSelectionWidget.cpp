// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BSCharacterSelectionWidget.h"

#include "Core/BSCharacterDefinition.h"
#include "GameModes/BSGameState.h"
#include "Player/BSPlayerState.h"
#include "SubSystem/BSPlayerUISubSystem.h"

void UBSCharacterSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	EllieButton->OnPressed.AddDynamic(this, &UBSCharacterSelectionWidget::OnEllieButtonPressed);
	JoelButton->OnPressed.AddDynamic(this, &UBSCharacterSelectionWidget::OnJoelButtonPressed);
}

void UBSCharacterSelectionWidget::OnEllieButtonPressed()
{
	FGameplayTag CharacterTag = FGameplayTag::RequestGameplayTag(FName("Character.Ellie"));
	SetCharacterDefinition(CharacterTag);
}

void UBSCharacterSelectionWidget::OnJoelButtonPressed()
{
	FGameplayTag CharacterTag = FGameplayTag::RequestGameplayTag(FName("Character.Joel"));
	SetCharacterDefinition(CharacterTag);
}

void UBSCharacterSelectionWidget::SetCharacterDefinition(FGameplayTag InTag)
{
	if (const ABSGameState* GameState = Cast<ABSGameState>(GetWorld()->GetGameState()))
	{
		GameState->CharacterDefManagerComponent->SetCharacterDefinition(GetOwningPlayerState(), InTag);
		RemoveFromParent();
	}
}
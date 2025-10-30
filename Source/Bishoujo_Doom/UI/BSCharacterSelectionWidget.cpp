// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BSCharacterSelectionWidget.h"

#include "Core/BSCharacterDefinition.h"
#include "GameModes/BSGameState.h"
#include "GameFramework/PlayerState.h"
#include "SubSystem/BSPlayerUISubSystem.h"

void UBSCharacterSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SpidyButton->OnPressed.AddDynamic(this, &UBSCharacterSelectionWidget::OnSpidyButtonPressed);
	DefaultButton->OnPressed.AddDynamic(this, &UBSCharacterSelectionWidget::OnDefaultButtonPressed);
}

void UBSCharacterSelectionWidget::NativeDestruct()
{
	Super::NativeDestruct();

	SpidyButton->OnPressed.Clear();
	DefaultButton->OnPressed.Clear();
}

void UBSCharacterSelectionWidget::OnSpidyButtonPressed()
{
	FGameplayTag CharacterTag = FGameplayTag::RequestGameplayTag(FName("Character.Spidy"));
	SetCharacterDefinition(CharacterTag);
}

void UBSCharacterSelectionWidget::OnDefaultButtonPressed()
{
	FGameplayTag CharacterTag = FGameplayTag::RequestGameplayTag(FName("Character.Default"));
	SetCharacterDefinition(CharacterTag);
}

void UBSCharacterSelectionWidget::SetCharacterDefinition(FGameplayTag InTag)
{
	UBSPlayerUISubSystem::Get(GetWorld())->SetGameInputModeOnly(GetPlayerContext().GetPlayerController());
	
	if (const ABSGameState* GameState = Cast<ABSGameState>(GetWorld()->GetGameState()))
	{
		const auto CharacterDefSystem = GetGameInstance<UGameInstance>()->GetSubsystem<UBSCharacterDefSystem>();
		CharacterDefSystem->SetCharacterDefinition(GetOwningPlayerState(), InTag);
		UBSPlayerUISubSystem::Get(GetWorld())->RemoveWidget(CharacterSelection);
	}
}
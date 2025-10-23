// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BSCharacterSelectionWidget.h"

#include "Core/BSCharacterDefinition.h"
#include "GameModes/BSGameState.h"
#include "GameFramework/PlayerState.h"
#include "SubSystem/BSPlayerUISubSystem.h"

void UBSCharacterSelectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	EllieButton->OnPressed.AddDynamic(this, &UBSCharacterSelectionWidget::OnEllieButtonPressed);
	JoelButton->OnPressed.AddDynamic(this, &UBSCharacterSelectionWidget::OnJoelButtonPressed);
}

void UBSCharacterSelectionWidget::NativeDestruct()
{
	Super::NativeDestruct();

	EllieButton->OnPressed.Clear();
	JoelButton->OnPressed.Clear();
}

void UBSCharacterSelectionWidget::OnEllieButtonPressed()
{
	FGameplayTag CharacterTag = FGameplayTag::RequestGameplayTag(FName("Character.Ellie"));
	SetCharacterDefinition(CharacterTag);
}

void UBSCharacterSelectionWidget::OnJoelButtonPressed()
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
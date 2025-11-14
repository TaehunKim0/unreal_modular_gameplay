// Fill out your copyright notice in the Description page of Project Settings.


#include "BSPlayerUISubSystem.h"

#include "Etc/BSLogChannels.h"
#include "AbilitySystem/BSAbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "Core/BSCharacterDefinition.h"
#include "Player/BSPlayerState.h"
#include "UI/Debug/BSDebugWidget.h"

void UBSPlayerUISubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogBS, Log, TEXT("UBSPlayerUISubSystem::FirstInitialize"));
}

void UBSPlayerUISubSystem::Deinitialize()
{
	RemoveAllWidgets();

	UE_LOG(LogBS, Log, TEXT("UBSPlayerUISubSystem::Deinitialize"));
	
	Super::Deinitialize();
}

void UBSPlayerUISubSystem::ShowWidget(EUICategory InCategory) const
{
	if (UUserWidget* Widget = GetWidgetByCategory(InCategory))
	{
		Widget->SetVisibility(ESlateVisibility::Visible);
	}
}

void UBSPlayerUISubSystem::RemoveWidget(EUICategory InCategory)
{
	if (UUserWidget* Widget = GetWidgetByCategory(InCategory))
	{
		Widget->RemoveFromParent();
		ActiveWidgetMap.Remove(InCategory);
	}
}


void UBSPlayerUISubSystem::HideWidget(EUICategory InCategory)
{
	if (UUserWidget* Widget = GetWidgetByCategory(InCategory))
	{
		Widget->SetVisibility(ESlateVisibility::Hidden);
	}
}

UUserWidget* UBSPlayerUISubSystem::GetWidgetByCategory(EUICategory InCategory) const
{
	if (ActiveWidgetMap.IsEmpty())
	{
		return nullptr;
	}
	
	if (const auto FoundWidget = ActiveWidgetMap.Find(InCategory))
	{
		return *FoundWidget;
	}

	return nullptr;
}

void UBSPlayerUISubSystem::ShowDebugMessage(const FString& InVariableName, const FString& InDesc)
{
	if (UUserWidget* Widget = GetWidgetByCategory(Debug))
	{
		if (UBSDebugWidget* const DebugWidget = Cast<UBSDebugWidget>(Widget))
		{
			DebugWidget->ShowDebugMessage(InVariableName, InDesc);
		}
		else
		{
			UE_LOG(LogBS, Error, TEXT("UBSPlayerUISubSystem::DebugWidget is Null"));
		}
	}
}

void UBSPlayerUISubSystem::ShowPawnAbilitySetMessage(const ABSPlayerState* InBSPlayerState,	const UBSCharacterDefinition* InNewDefinition)
{
	UBSDebugWidget* DebugWidget = GetWidget<UBSDebugWidget>(EUICategory::Debug);
	if (DebugWidget)
	{
		DebugWidget->ClearDebugMessages();
	}

	if (InNewDefinition)
		ShowDebugMessage("DefinitionName",  InNewDefinition->CharacterTag.ToString());

	// Abilities
	int Index = 1;
	for (const auto AbilitySpec : InBSPlayerState->GetBSAbilitySystemComponent()->GetActivatableAbilities())
	{
		if (AbilitySpec.Ability)
		{
			FString AbilityName = AbilitySpec.Ability->GetName();
			FString VarName = "Ability "; VarName.AppendInt(Index);
			ShowDebugMessage(VarName,  AbilityName);
			Index += 1;
		}
	}

	// Attributes
	Index = 1;
	const auto SpawnedAttributes = InBSPlayerState->GetBSAbilitySystemComponent()->GetSpawnedAttributes();
	for (const auto AttributeSet : SpawnedAttributes)
	{
		if (AttributeSet)
		{
			FString AttributeName = AttributeSet->GetName();
			FString VarName = "Attribute "; VarName.AppendInt(Index);
			ShowDebugMessage(VarName,  AttributeName);
		}
	}

}

void UBSPlayerUISubSystem::K2_ShowPawnAbilitySetMessage(APawn* InPawn)
{
	if (InPawn)
	{
		auto BSPS =	Cast<ABSPlayerState>(InPawn->GetPlayerState());
		
		if (!BSPS) return;

		auto Def = BSPS->GetCharacterDefData();
		
		ShowPawnAbilitySetMessage(BSPS, Def);
	}
}

void UBSPlayerUISubSystem::SetUIInputModeOnly(UUserWidget* InFocusWidget, APlayerController* InPlayerController)
{
	FInputModeUIOnly InputModeUI;
	InputModeUI.SetWidgetToFocus(InFocusWidget->TakeWidget());
	InputModeUI.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InPlayerController->SetInputMode(InputModeUI);
	InPlayerController->bShowMouseCursor = true;
}

void UBSPlayerUISubSystem::SetGameInputModeOnly(APlayerController* InPlayerController)
{
	FInputModeGameOnly InputModeGameOnly;
	InPlayerController->SetInputMode(InputModeGameOnly);
	InPlayerController->bShowMouseCursor = false;
}

void UBSPlayerUISubSystem::RemoveAllWidgets()
{
	for (auto& WidgetPair : ActiveWidgetMap)
	{
		if (WidgetPair.Value)
		{
			WidgetPair.Value->RemoveFromParent();
		}
	}
    
	ActiveWidgetMap.Empty();
}


UUserWidget* UBSPlayerUISubSystem::K2_CreateWidget_Implementation(TSubclassOf<UUserWidget> InWidgetClass,
	EUICategory InCategory, APlayerController* InPlayerController)
{
	return CreateWidget<UUserWidget>(InWidgetClass, InCategory, InPlayerController);
}

UBSPlayerUISubSystem* UBSPlayerUISubSystem::Get(const UObject* WorldContext)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			if (ULocalPlayer* LocalPlayer = GameInstance->GetFirstGamePlayer())
			{
				return LocalPlayer->GetSubsystem<UBSPlayerUISubSystem>();
			}
		}
	}

	UE_LOG(LogBS, Log, TEXT("UBSPlayerUISubSystem::Get is  nullptr"));
	
	return nullptr;
}
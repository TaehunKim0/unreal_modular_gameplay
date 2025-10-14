// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BSPlayerState.h"
#include "ModularPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "BSPlayerController.generated.h"

class ABSPlayerState;
class UBSAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class BISHOUJO_DOOM_API ABSPlayerController : public AModularPlayerController
{
	GENERATED_BODY()

public:
	ABSPlayerController();

	// PlayerController interface~
	virtual void OnPossess(APawn* InPawn) override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void ProcessPlayerInput(const float DeltaTime, const bool bGamePaused) override;
	// end
	
public:
	UFUNCTION(Exec)
	void CheckGameFeatureStatus();

	UBSAbilitySystemComponent* GetBSAbilitySystemComponent() const;
	ABSPlayerState* GetBSPlayerState() const;
};

inline ABSPlayerState* ABSPlayerController::GetBSPlayerState() const
{
	return CastChecked<ABSPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

inline UBSAbilitySystemComponent* ABSPlayerController::GetBSAbilitySystemComponent() const
{
	const ABSPlayerState* BSPS = GetBSPlayerState();
	return (BSPS ? BSPS->GetBSAbilitySystemComponent() : nullptr);
}

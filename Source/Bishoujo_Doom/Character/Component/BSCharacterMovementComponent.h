// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BSCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class BISHOUJO_DOOM_API UBSCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	UBSCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);
	
	virtual void SimulateMovement(float DeltaTime) override;
	virtual bool CanAttemptJump() const override;

	//~UMovementComponent interface
	virtual FRotator GetDeltaRotation(float DeltaTime) const override;
	virtual float GetMaxSpeed() const override;
	//~End of UMovementComponent interface

protected:
	virtual void InitializeComponent() override;
};

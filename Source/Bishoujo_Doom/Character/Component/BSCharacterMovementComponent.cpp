// Fill out your copyright notice in the Description page of Project Settings.


#include "BSCharacterMovementComponent.h"

UBSCharacterMovementComponent::UBSCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
{
}

void UBSCharacterMovementComponent::SimulateMovement(float DeltaTime)
{
	Super::SimulateMovement(DeltaTime);
}

bool UBSCharacterMovementComponent::CanAttemptJump() const
{
	return IsJumpAllowed() &&
		(IsMovingOnGround() || IsFalling()); // Falling included for double-jump and non-zero jump hold time, but validated by character.
}

FRotator UBSCharacterMovementComponent::GetDeltaRotation(float DeltaTime) const
{
	return Super::GetDeltaRotation(DeltaTime);
}

float UBSCharacterMovementComponent::GetMaxSpeed() const
{
	return Super::GetMaxSpeed();
}

void UBSCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

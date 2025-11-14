// Fill out your copyright notice in the Description page of Project Settings.


#include "BSCharacterMovementComponent.h"

#include "Etc/BSLogChannels.h"
#include "DataRegistrySubsystem.h"
#include "DataRegistryId.h"
#include "Data/MovementSettingsData.h"

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

	ApplyMovementSettings();
}

void UBSCharacterMovementComponent::ApplyMovementSettings()
{
	const UDataRegistrySubsystem* DRSubsystem = UDataRegistrySubsystem::Get();
	if (!DRSubsystem)
		return;

	const FDataRegistryId ItemId(TEXT("Movement"),	TEXT("Default"));
	// const FMovementSettingsData* Data = DRSubsystem->GetCachedItem<FMovementSettingsData>(ItemId);
	//
	// if (Data)
	// {
	// 	UE_LOG(LogBS, Log, TEXT("UBSCharacterMovementComponent: FMovementSettingsData is valid"));
	// }
	
	DRSubsystem->AcquireItem(ItemId, FDataRegistryItemAcquiredCallback::CreateLambda([this](const FDataRegistryAcquireResult& Result) {
		auto Data = Result.GetItem<FMovementSettingsData>();
		if (Data == nullptr)
		{
			UE_LOG(LogBS, Error, TEXT("UBSCharacterMovementComponent: FMovementSettingsData is nullptr"));
			return;
		}
	
		// Data Asset에서 설정 가져오기
		const FMovementSpeedSettings& Speed = Data->SpeedSettings;
		const FMovementAccelerationSettings& Accel = Data->AccelerationSettings;
		const FMovementRotationSettings& Rotation = Data->RotationSettings;

		// 속도 설정
		MaxWalkSpeed = Speed.WalkSpeed;
    
		// 가속/감속 설정
		MaxAcceleration = Accel.MaxAcceleration;
		BrakingDecelerationWalking = Accel.BrakingDeceleration;
		GroundFriction = Accel.GroundFriction;
    
		// 회전 설정
		RotationRate = FRotator(0.0f, Rotation.RotationRate, 0.0f);
		bOrientRotationToMovement = Rotation.bOrientRotationToMovement;
		bUseControllerDesiredRotation = Rotation.bUseControllerDesiredRotation;

		UE_LOG(LogBS, Log, TEXT("UBSCharacterMovementComponent: Movement settings applied from Data Asset"));
	}));
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GameFeatureActionFilter.h"

#include "BSLogChannels.h"
#include "Player/BSPlayerState.h"

FGameFeatureActionFilter::FGameFeatureActionFilter()
{
}

FGameFeatureActionFilter::~FGameFeatureActionFilter()
{
}

bool FGameFeatureActionFilter::CanApplyFeatureAction(FString& InPluginName, const APlayerState* InTargetPlayerState)
{
	if (!IsValid(InTargetPlayerState))
	{
		UE_LOG(LogBS, Error, TEXT("CanApplyFeatureAction::Target PlayerState is not valid"));
		return false;
	}

	const auto BSPlayerState = Cast<ABSPlayerState>(InTargetPlayerState);
	if (!IsValid(BSPlayerState))
	{
		UE_LOG(LogBS, Error, TEXT("CanApplyFeatureAction::Target PlayerState Cast Failed"));
		return false;
	}

	if (BSPlayerState->PendingCharacterPluginName == InPluginName)
	{
		UE_LOG(LogBS, Log, TEXT("CanApplyFeatureAction::Target PlayerState Can Applying"));
		return true;
	}

	if (BSPlayerState->PendingCharacterPluginName.IsEmpty())
	{
		UE_LOG(LogBS, Error, TEXT("CanApplyFeatureAction::PendingCharacterPluginName is Empty"));
	}

	return false;
}

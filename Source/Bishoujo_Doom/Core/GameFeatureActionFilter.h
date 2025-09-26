// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class BISHOUJO_DOOM_API FGameFeatureActionFilter
{
public:
	FGameFeatureActionFilter();
	~FGameFeatureActionFilter();

public:
	static bool CanApplyFeatureAction(FString &InPluginName, const APlayerState* InTargetPlayerState);
};

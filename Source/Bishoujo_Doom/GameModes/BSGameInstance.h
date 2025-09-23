// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BSGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class BISHOUJO_DOOM_API UBSGameInstance : public UGameInstance
{
	GENERATED_BODY()

protected:

	virtual void Init() override;
	virtual void StartGameInstance() override;
	virtual void Shutdown() override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TestCharacter.h"
#include "GameFramework/Actor.h"
#include "TestCharacterManager.generated.h"

UCLASS()
class BISHOUJO_DOOM_API ATestCharacterManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ATestCharacterManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<ATestCharacter> TestClass;
};
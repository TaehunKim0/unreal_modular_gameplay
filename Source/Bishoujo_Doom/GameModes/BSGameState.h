// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularGameState.h"
#include "Core/BSCharacterDefManagerComponent.h"
#include "BSGameState.generated.h"

struct FStreamableHandle;
class UBSCharacterDefinition;

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class BISHOUJO_DOOM_API ABSGameState : public AModularGameStateBase
{
	GENERATED_BODY()
public:
	ABSGameState();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	UPROPERTY()
	TObjectPtr<UBSCharacterDefManagerComponent> CharacterDefManagerComponent;

public:
	// 기본 캐릭터 정의
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ABSGameState")
	TObjectPtr<const UBSCharacterDefinition> DefaultCharacterDefinition;
	
protected:
	// 사용 가능한 모든 캐릭터 정의들
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ABSGameState")
	TArray<TSoftObjectPtr<const UBSCharacterDefinition>> AvailableCharacterDefinitions;
};
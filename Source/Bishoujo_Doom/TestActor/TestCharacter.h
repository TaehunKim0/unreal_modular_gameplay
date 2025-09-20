// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestCharacter.generated.h"

UCLASS()
class BISHOUJO_DOOM_API ATestCharacter : public AActor
{
	GENERATED_BODY()
	
public:	
	ATestCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnCharacterAssetsLoaded();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSoftObjectPtr<USkeletalMesh> WeaponMesh;
	
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	// USkeletalMesh* WeaponMesh;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* MeshComponent;
};

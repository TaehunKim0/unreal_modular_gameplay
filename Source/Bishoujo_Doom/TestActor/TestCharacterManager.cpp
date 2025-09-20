// Fill out your copyright notice in the Description page of Project Settings.


#include "TestActor/TestCharacterManager.h"

// Sets default values
ATestCharacterManager::ATestCharacterManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATestCharacterManager::BeginPlay()
{
	Super::BeginPlay();

	if (TestClass.Get() != nullptr)
		GetWorld()->SpawnActor(TestClass.Get());
}

// Called every frame
void ATestCharacterManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


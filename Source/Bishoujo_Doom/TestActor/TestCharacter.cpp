// Fill out your copyright notice in the Description page of Project Settings.


#include "TestActor/TestCharacter.h"

#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

// Sets default values
ATestCharacter::ATestCharacter()
{
	if (WeaponMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("Already Loaded"));
	}
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
}

// Called when the game starts or when spawned
void ATestCharacter::BeginPlay()
{
	Super::BeginPlay();

	MeshComponent->SetSkeletalMesh(WeaponMesh.LoadSynchronous());
}

// Called every frame
void ATestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
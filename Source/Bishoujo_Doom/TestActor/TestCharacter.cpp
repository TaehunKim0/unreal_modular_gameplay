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

	UE_LOG(LogTemp, Warning, TEXT("ATestWeapon 생성됨"));
}

// Called when the game starts or when spawned
void ATestCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 로드할 에셋들의 경로를 수집
	TArray<FSoftObjectPath> AssetsToLoad;

	if (!WeaponMesh.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("[SOFT_REF] 스켈레탈 메시 1개 로딩중....."));
		UE_LOG(LogTemp, Warning, TEXT("[SOFT_REF] 로딩 시작 시간 : %f"),GetWorld()->GetTimeSeconds());
		auto SkeletalMeshAsset = WeaponMesh.LoadSynchronous();

		if (SkeletalMeshAsset)
		{
			UE_LOG(LogTemp, Warning, TEXT("SkeletalMesh 동기 로딩 완료"));
			UE_LOG(LogTemp, Warning, TEXT("[SOFT_REF] 로딩 완료 시간 : %f"),GetWorld()->GetTimeSeconds());
		}
	}
    
	// if (!WeaponMesh.IsNull())
	// {
	// 	AssetsToLoad.Add(WeaponMesh.ToSoftObjectPath());
	// }
	//
	// if (AssetsToLoad.Num() > 0)
	// {
	// 	// 비동기 로딩 시작
	// 	UE_LOG(LogTemp, Warning, TEXT("[SOFT_REF] 스켈레탈 메시 %d개 로딩중....."), AssetsToLoad.Num());
	// 	UE_LOG(LogTemp, Warning, TEXT("[SOFT_REF] 로딩 시작 시간 : %f"),GetWorld()->GetTimeSeconds());
	// 	
	// 	UAssetManager& AssetManager = UAssetManager::Get();
	// 	FStreamableManager& StreamableManager = AssetManager.GetStreamableManager();
 //        
	// 	auto AllAssetsLoadHandle = StreamableManager.RequestAsyncLoad(
	// 		AssetsToLoad,
	// 		FStreamableDelegate::CreateUFunction(this, FName("OnCharacterAssetsLoaded"))
	// 	);
	//
	// 	UE_LOG(LogTemp, Warning, TEXT("[SOFT_REF] 로딩중에도 게임 계속 실행중....."));
	// }

	
}

// Called every frame
void ATestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATestCharacter::OnCharacterAssetsLoaded()
{
	UE_LOG(LogTemp, Warning, TEXT("[SOFT_REF] 캐릭터의 스켈레탈 메시 로딩 완료"));
	UE_LOG(LogTemp, Warning, TEXT("[SOFT_REF] 로딩 완료 시간 : %f"),GetWorld()->GetTimeSeconds());

	if (WeaponMesh.IsValid())
	{
		MeshComponent->SetSkeletalMesh(WeaponMesh.Get());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[SOFT_REF] Weapon Mesh is unvalid."));
	}
}
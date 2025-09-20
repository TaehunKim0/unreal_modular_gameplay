#include "BSGameMode.h"
#include "BSGameState.h"
#include "BSLogChannels.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Core/BSCharacterDefinition.h"
#include "Player/BSPlayerState.h"

ABSGameMode::ABSGameMode()
{
	// 기본 클래스 설정
	PlayerStateClass = ABSPlayerState::StaticClass();
	GameStateClass = ABSGameState::StaticClass();

	UE_LOG(LogBS, Log, TEXT("ABSGameMode::ABSGameMode"));
}

void ABSGameMode::StartPlay()
{
	Super::StartPlay();
}

void ABSGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
}

void ABSGameMode::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	UE_LOG(LogBS, Log, TEXT("ABSGameMode::PreInitializeComponents"));
}

void ABSGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	UE_LOG(LogBS, Log, TEXT("ABSGameMode::PostInitializeComponents"));
}

void ABSGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
    
	// PlayerState가 생성되고 초기화된 후 기본 캐릭터 정의 설정
	if (ABSPlayerState* BSPlayerState = NewPlayer->GetPlayerState<ABSPlayerState>())
	{
		UE_LOG(LogBS, Log, TEXT("ABSGameMode::PostLogin"));

		GetGameState<ABSGameState>()->CharacterDefManagerComponent->SetCharacterDefinition(BSPlayerState, TestDef);
	}
}
void ABSGameMode::BeginPlay()
{
	Super::BeginPlay();
}
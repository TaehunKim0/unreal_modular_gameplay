#include "GameFeatures/GameFeatureAction_AddInputContextMapping.h"

#include "BSLogChannels.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/AssetManager.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFeaturesSubsystem.h"

void UGameFeatureAction_AddInputContextMapping::OnGameFeatureRegistering()
{
    Super::OnGameFeatureRegistering();

    // AssetManager를 통해 InputMappingContext 에셋 로드 시작
    UAssetManager& AssetManager = UAssetManager::Get();
    
    for (const TSoftObjectPtr<UInputMappingContext>& MappingPtr : InputMappings)
    {
        if (!MappingPtr.IsNull())
        {
            // 비동기 로드 요청 (더 효율적)
            AssetManager.GetStreamableManager().RequestAsyncLoad(
                MappingPtr.ToSoftObjectPath(),
                FStreamableDelegate(),
                FStreamableManager::AsyncLoadHighPriority
            );
        }
    }
}

void UGameFeatureAction_AddInputContextMapping::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
    Super::OnGameFeatureActivating(Context);
    // World 확인
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }
    
    // GameInstance도 필요하다면
    UGameInstance* GameInstance = World->GetGameInstance();
    if (!GameInstance)
    {
        return;
    }
    
    // 모든 LocalPlayer에 직접 접근하는 방법
    for (int32 PlayerIndex = 0; PlayerIndex < GameInstance->GetNumLocalPlayers(); ++PlayerIndex)
    {
        if (ULocalPlayer* LocalPlayer = GameInstance->GetLocalPlayerByIndex(PlayerIndex))
        {
            AddInputMappingForPlayer(LocalPlayer);
        }
    }

    UE_LOG(LogBS, Warning, TEXT("UGameFeatureAction_AddInputContextMapping::OnGameFeatureActivating"));
}

void UGameFeatureAction_AddInputContextMapping::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
    Super::OnGameFeatureDeactivating(Context);
    
    // 모든 활성 플레이어에서 Input Mapping 제거
    if (UWorld* World = GetWorld())
    {
        for (FConstPlayerControllerIterator PCIterator = World->GetPlayerControllerIterator(); PCIterator; ++PCIterator)
        {
            if (APlayerController* PC = PCIterator->Get())
            {
                RemoveInputMapping(PC);
            }
        }
    }
}

void UGameFeatureAction_AddInputContextMapping::OnGameFeatureUnregistering()
{
    Super::OnGameFeatureUnregistering();
    
    // AssetManager를 통해 리소스 해제
    UAssetManager& AssetManager = UAssetManager::Get();
    
    for (const TSoftObjectPtr<UInputMappingContext>& MappingPtr : InputMappings)
    {
        if (!MappingPtr.IsNull())
        {
            // 스트리밍 핸들 해제 (메모리 정리)
            AssetManager.GetStreamableManager().Unload(MappingPtr.ToSoftObjectPath());
        }
    }
}

void UGameFeatureAction_AddInputContextMapping::AddInputMappingForPlayer(UPlayer* Player)
{
    if (!Player)
    {
        return;
    }
    
    ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
    if (!LocalPlayer)
    {
        return;
    }
    
    UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
    if (!InputSubsystem)
    {
        return;
    }
    
    UAssetManager& AssetManager = UAssetManager::Get();
    
    // 모든 InputMappingContext를 추가
    for (const TSoftObjectPtr<UInputMappingContext>& MappingPtr : InputMappings)
    {
        if (!MappingPtr.IsNull())
        {
            // AssetManager를 통해 에셋 가져오기
            if (UInputMappingContext* IMC = AssetManager.GetStreamableManager().LoadSynchronous<UInputMappingContext>(MappingPtr.ToSoftObjectPath()))
            {
                // Priority 0으로 추가 (필요시 설정 가능하도록 확장 가능)
                InputSubsystem->AddMappingContext(IMC, 0);
            }
        }
    }
}

void UGameFeatureAction_AddInputContextMapping::RemoveInputMapping(APlayerController* PlayerController)
{
    if (!PlayerController)
    {
        return;
    }
    
    ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PlayerController->GetLocalPlayer());
    if (!LocalPlayer)
    {
        return;
    }
    
    UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
    if (!InputSubsystem)
    {
        return;
    }
    
    UAssetManager& AssetManager = UAssetManager::Get();
    
    // 모든 InputMappingContext를 제거
    for (const TSoftObjectPtr<UInputMappingContext>& MappingPtr : InputMappings)
    {
        if (!MappingPtr.IsNull())
        {
            // AssetManager를 통해 에셋 가져오기
            if (UInputMappingContext* IMC = AssetManager.GetStreamableManager().LoadSynchronous<UInputMappingContext>(MappingPtr.ToSoftObjectPath()))
            {
                InputSubsystem->RemoveMappingContext(IMC);
            }
        }
    }
}
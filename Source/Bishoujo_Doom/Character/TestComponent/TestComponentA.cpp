// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TestComponent/TestComponentA.h"
#include "BSGamePlayTags.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFramework/Character.h"

const FName UTestComponentA::NAME_TestComponentA("TestComponentA");

UTestComponentA::UTestComponentA(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryComponentTick.bCanEverTick = false;
    PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UTestComponentA::OnRegister()
{
    Super::OnRegister();
    UE_LOG(LogBSInitState, Warning, TEXT("[TestComponentA] OnRegister - 컴포넌트 등록"));
    
    RegisterInitStateFeature();
}

void UTestComponentA::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogBSInitState, Warning, TEXT("[TestComponentA] BeginPlay - 게임 시작"));
    
    // 다른 컴포넌트들의 상태 변화 감시
    BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);
    
    // Spawned 상태로 전환 시도
    ensure(TryToChangeInitState(BSGamePlayTags::InitState_Spawned));
    CheckDefaultInitialization();
}

void UTestComponentA::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UnregisterInitStateFeature();
    Super::EndPlay(EndPlayReason);
}

bool UTestComponentA::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
    UE_LOG(LogBSInitState, Warning, TEXT("[TestComponentA] CanChangeInitState: %s → %s"), 
           *CurrentState.ToString(), *DesiredState.ToString());

    // Spawned 단계: 기본적으로 허용
    if (!CurrentState.IsValid() && DesiredState == BSGamePlayTags::InitState_Spawned)
    {
        return true;
    }
    
    // DataAvailable 단계: 데이터 로딩 완료 확인
    if (CurrentState == BSGamePlayTags::InitState_Spawned && DesiredState == BSGamePlayTags::InitState_CharacterDefinitionLoaded)
    {
        return bDataLoaded;
    }
    
    // DataInitialized 단계: 모든 컴포넌트가 DataAvailable이어야 함
    if (CurrentState == BSGamePlayTags::InitState_CharacterDefinitionLoaded && DesiredState == BSGamePlayTags::InitState_CharacterInitialized)
    {
        return Manager->HaveAllFeaturesReachedInitState(GetPawn<ACharacter>(), BSGamePlayTags::InitState_CharacterDefinitionLoaded);
    }
    
    // GameplayReady 단계: 모든 컴포넌트가 DataInitialized이어야 함
    if (CurrentState == BSGamePlayTags::InitState_CharacterInitialized && DesiredState == BSGamePlayTags::InitState_GameplayReady)
    {
        return Manager->HaveAllFeaturesReachedInitState(GetPawn<ACharacter>(), BSGamePlayTags::InitState_CharacterInitialized);
    }

    return false;
}

void UTestComponentA::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
    UE_LOG(LogBSInitState, Error, TEXT("[TestComponentA] 상태 변경 처리: %s → %s"), 
           *CurrentState.ToString(), *DesiredState.ToString());

    if (DesiredState == BSGamePlayTags::InitState_Spawned)
    {
        UE_LOG(LogBSInitState, Error, TEXT("[TestComponentA] ✅ Spawned - 데이터 로딩 시작"));
        LoadMyData();
    }
    else if (DesiredState == BSGamePlayTags::InitState_CharacterDefinitionLoaded)
    {
        UE_LOG(LogBSInitState, Error, TEXT("[TestComponentA] ✅ DataAvailable - 데이터 준비 완료"));
    }
    else if (DesiredState == BSGamePlayTags::InitState_CharacterInitialized)
    {
        UE_LOG(LogBSInitState, Error, TEXT("[TestComponentA] ✅ DataInitialized - 시스템 초기화"));
        InitializeMySystem();
    }
    else if (DesiredState == BSGamePlayTags::InitState_GameplayReady)
    {
        UE_LOG(LogBSInitState, Error, TEXT("[TestComponentA] ✅ GameplayReady - 게임플레이 준비 완료!"));
    }
}

void UTestComponentA::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
    if (Params.FeatureName != NAME_TestComponentA)
    {
        UE_LOG(LogBSInitState, Log, TEXT("[TestComponentA] 다른 컴포넌트 상태 변화 감지: %s → %s"), 
               *Params.FeatureName.ToString(), *Params.FeatureState.ToString());
        
        // 다른 컴포넌트가 변화하면 자신도 다음 단계 진행 시도
        CheckDefaultInitialization();
    }
}

void UTestComponentA::CheckDefaultInitialization()
{
    static const TArray<FGameplayTag> StateChain = {
        BSGamePlayTags::InitState_Spawned,
        BSGamePlayTags::InitState_CharacterDefinitionLoaded,
        BSGamePlayTags::InitState_CharacterInitialized,
        BSGamePlayTags::InitState_GameplayReady
    };

    ContinueInitStateChain(StateChain);
}

void UTestComponentA::LoadMyData()
{
    // 데이터 로딩 시뮬레이션 (즉시 완료)
    bDataLoaded = true;
    UE_LOG(LogBSInitState, Warning, TEXT("[TestComponentA] 데이터 로딩 완료"));
    CheckDefaultInitialization();
}

void UTestComponentA::InitializeMySystem()
{
    UE_LOG(LogBSInitState, Warning, TEXT("[TestComponentA] 시스템 초기화 완료"));
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/TestComponent/TestComponentB.h"
#include "BSGamePlayTags.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFramework/Character.h"

const FName UTestComponentB::NAME_TestComponentB("TestComponentB");

UTestComponentB::UTestComponentB(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryComponentTick.bCanEverTick = false;
    PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UTestComponentB::OnRegister()
{
    Super::OnRegister();
    UE_LOG(LogBSInitState, Warning, TEXT("[TestComponentB] OnRegister - 컴포넌트 등록"));
    
    RegisterInitStateFeature();
}

void UTestComponentB::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogBSInitState, Warning, TEXT("[TestComponentB] BeginPlay - 게임 시작"));
    
    BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);
    
    ensure(TryToChangeInitState(BSGamePlayTags::InitState_Spawned));
    CheckDefaultInitialization();
}

void UTestComponentB::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UnregisterInitStateFeature();
    Super::EndPlay(EndPlayReason);
}

bool UTestComponentB::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
    UE_LOG(LogBSInitState, Warning, TEXT("[TestComponentB] CanChangeInitState: %s → %s"), 
           *CurrentState.ToString(), *DesiredState.ToString());

    if (!CurrentState.IsValid() && DesiredState == BSGamePlayTags::InitState_Spawned)
    {
        return true;
    }
    
    // B는 A가 DataAvailable이 된 후에만 자신도 DataAvailable이 될 수 있음
    if (CurrentState == BSGamePlayTags::InitState_Spawned && DesiredState == BSGamePlayTags::InitState_DataAvailable)
    {
        // A 컴포넌트가 준비되었는지 확인
        bool bComponentAReady = Manager->HasFeatureReachedInitState(GetPawn<ACharacter>(), FName("TestComponentA"), BSGamePlayTags::InitState_DataAvailable);
        return bConfigLoaded && bComponentAReady;
    }
    
    if (CurrentState == BSGamePlayTags::InitState_DataAvailable && DesiredState == BSGamePlayTags::InitState_DataInitialized)
    {
        return Manager->HaveAllFeaturesReachedInitState(GetPawn<ACharacter>(), BSGamePlayTags::InitState_DataAvailable);
    }
    
    if (CurrentState == BSGamePlayTags::InitState_DataInitialized && DesiredState == BSGamePlayTags::InitState_GameplayReady)
    {
        return Manager->HaveAllFeaturesReachedInitState(GetPawn<ACharacter>(), BSGamePlayTags::InitState_DataInitialized);
    }

    return false;
}

void UTestComponentB::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
    UE_LOG(LogBSInitState, Error, TEXT("[TestComponentB] 상태 변경 처리: %s → %s"), 
           *CurrentState.ToString(), *DesiredState.ToString());

    if (DesiredState == BSGamePlayTags::InitState_Spawned)
    {
        UE_LOG(LogBSInitState, Error, TEXT("[TestComponentB] ✅ Spawned - 설정 로딩 시작 (A 대기중...)"));
        LoadConfiguration();
    }
    else if (DesiredState == BSGamePlayTags::InitState_DataAvailable)
    {
        UE_LOG(LogBSInitState, Error, TEXT("[TestComponentB] ✅ DataAvailable - A 의존성 확인 후 준비 완료"));
    }
    else if (DesiredState == BSGamePlayTags::InitState_DataInitialized)
    {
        UE_LOG(LogBSInitState, Error, TEXT("[TestComponentB] ✅ DataInitialized - 기능 설정"));
        SetupMyFeatures();
    }
    else if (DesiredState == BSGamePlayTags::InitState_GameplayReady)
    {
        UE_LOG(LogBSInitState, Error, TEXT("[TestComponentB] ✅ GameplayReady - 게임플레이 준비 완료!"));
    }
}

void UTestComponentB::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
    if (Params.FeatureName != NAME_TestComponentB)
    {
        UE_LOG(LogBSInitState, Log, TEXT("[TestComponentB] 다른 컴포넌트 상태 변화 감지: %s → %s"), 
               *Params.FeatureName.ToString(), *Params.FeatureState.ToString());
        
        CheckDefaultInitialization();
    }
}

void UTestComponentB::CheckDefaultInitialization()
{
    static const TArray<FGameplayTag> StateChain = {
        BSGamePlayTags::InitState_Spawned,
        BSGamePlayTags::InitState_DataAvailable,
        BSGamePlayTags::InitState_DataInitialized,
        BSGamePlayTags::InitState_GameplayReady
    };

    ContinueInitStateChain(StateChain);
}

void UTestComponentB::LoadConfiguration()
{
    bConfigLoaded = true;
    UE_LOG(LogBSInitState, Warning, TEXT("[TestComponentB] 설정 로딩 완료 (하지만 A를 기다리는 중...)"));
    CheckDefaultInitialization();
}

void UTestComponentB::SetupMyFeatures()
{
    UE_LOG(LogBSInitState, Warning, TEXT("[TestComponentB] 기능 설정 완료"));
}
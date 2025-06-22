// ============================================================================
// 3. TestComponentC.cpp
// ============================================================================
#include "TestComponentC.h"
#include "BSGamePlayTags.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFramework/Character.h"

const FName UTestComponentC::NAME_TestComponentC("TestComponentC");

UTestComponentC::UTestComponentC(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryComponentTick.bCanEverTick = false;
    PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UTestComponentC::OnRegister()
{
    Super::OnRegister();
    UE_LOG(LogBSInitState, Warning, TEXT("[TestComponentC] OnRegister - 컴포넌트 등록"));
    
    RegisterInitStateFeature();
}

void UTestComponentC::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogBSInitState, Warning, TEXT("[TestComponentC] BeginPlay - 게임 시작"));
    
    BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);
    
    ensure(TryToChangeInitState(BSGamePlayTags::InitState_Spawned));
    CheckDefaultInitialization();
}

void UTestComponentC::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UnregisterInitStateFeature();
    Super::EndPlay(EndPlayReason);
}

bool UTestComponentC::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
    UE_LOG(LogBSInitState, Warning, TEXT("[TestComponentC] CanChangeInitState: %s → %s"), 
           *CurrentState.ToString(), *DesiredState.ToString());

    if (!CurrentState.IsValid() && DesiredState == BSGamePlayTags::InitState_Spawned)
    {
        return true;
    }
    
    // C는 A와 B가 모두 DataAvailable이 된 후에만 자신도 DataAvailable이 될 수 있음
    if (CurrentState == BSGamePlayTags::InitState_Spawned && DesiredState == BSGamePlayTags::InitState_DataAvailable)
    {
        bool bComponentAReady = Manager->HasFeatureReachedInitState(GetPawn<ACharacter>(), FName("TestComponentA"), BSGamePlayTags::InitState_DataAvailable);
        bool bComponentBReady = Manager->HasFeatureReachedInitState(GetPawn<ACharacter>(), FName("TestComponentB"), BSGamePlayTags::InitState_DataAvailable);
        
        return bResourcesLoaded && bComponentAReady && bComponentBReady;
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

void UTestComponentC::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
    UE_LOG(LogBSInitState, Error, TEXT("[TestComponentC] 상태 변경 처리: %s → %s"), 
           *CurrentState.ToString(), *DesiredState.ToString());

    if (DesiredState == BSGamePlayTags::InitState_Spawned)
    {
        UE_LOG(LogBSInitState, Error, TEXT("[TestComponentC] ✅ Spawned - 리소스 로딩 시작 (A, B 대기중...)"));
        LoadResources();
    }
    else if (DesiredState == BSGamePlayTags::InitState_DataAvailable)
    {
        UE_LOG(LogBSInitState, Error, TEXT("[TestComponentC] ✅ DataAvailable - A, B 의존성 확인 후 준비 완료"));
    }
    else if (DesiredState == BSGamePlayTags::InitState_DataInitialized)
    {
        UE_LOG(LogBSInitState, Error, TEXT("[TestComponentC] ✅ DataInitialized - 최종 설정"));
        FinalizeSetup();
    }
    else if (DesiredState == BSGamePlayTags::InitState_GameplayReady)
    {
        UE_LOG(LogBSInitState, Error, TEXT("[TestComponentC] ✅ GameplayReady - 모든 초기화 완료!"));
    }
}

void UTestComponentC::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
    if (Params.FeatureName != NAME_TestComponentC)
    {
        UE_LOG(LogBSInitState, Log, TEXT("[TestComponentC] 다른 컴포넌트 상태 변화 감지: %s → %s"), 
               *Params.FeatureName.ToString(), *Params.FeatureState.ToString());
        
        CheckDefaultInitialization();
    }
}

void UTestComponentC::CheckDefaultInitialization()
{
    static const TArray<FGameplayTag> StateChain = {
        BSGamePlayTags::InitState_Spawned,
        BSGamePlayTags::InitState_DataAvailable,
        BSGamePlayTags::InitState_DataInitialized,
        BSGamePlayTags::InitState_GameplayReady
    };

    ContinueInitStateChain(StateChain);
}

void UTestComponentC::LoadResources()
{
    bResourcesLoaded = true;
    UE_LOG(LogBSInitState, Warning, TEXT("[TestComponentC] 리소스 로딩 완료 (하지만 A, B를 기다리는 중...)"));
    CheckDefaultInitialization();
}

void UTestComponentC::FinalizeSetup()
{
    UE_LOG(LogBSInitState, Warning, TEXT("[TestComponentC] 최종 설정 완료"));
}

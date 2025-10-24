#include "GameFeatures/GameFeatureAction_AddInputContextMapping.h"

#include "BSLogChannels.h"
#include "EngineUtils.h"
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
}

void UGameFeatureAction_AddInputContextMapping::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
    Super::OnGameFeatureActivating(Context);

    for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
    {
        if (Context.ShouldApplyToWorldContext(WorldContext))
        {
            if (UWorld* World = WorldContext.World())
            {
                for (TActorIterator<APawn> It(World); It; ++It)
                {
                    APawn* Pawn = *It;
                    if (!IsValid(Pawn))
                    {
                        continue;
                    }

                    if (!Pawn->IsLocallyControlled())
                    {
                        continue;
                    }
                    
                    if (UClass* TargetActorClass = InputMapping.TargetActorClass.LoadSynchronous())
                    {
                        if (Pawn->IsA(TargetActorClass))
                        {
                            AddInputMappingForPlayer(Pawn);
                        }
                    }
                    else
                    {
                        UE_LOG(LogBS, Warning, TEXT("UGameFeatureAction_AddInputContextMapping:: 적용 중 타겟 액터 클래스 로드 실패: %s"), *InputMapping.TargetActorClass.ToString());
                    }
                }
            }
        }
    }
}

void UGameFeatureAction_AddInputContextMapping::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
    Super::OnGameFeatureDeactivating(Context);

    while (!AddedInputMappingMap.IsEmpty())
    {
        auto It = AddedInputMappingMap.CreateIterator();

        if (IsValid(It->Key) && It->Key->IsValidLowLevel())
        {
            if (Cast<APlayerController>(It->Key->GetController()))
            {
                RemoveInputMapping(It->Key);
                continue;
            }
        }
        
        It.RemoveCurrent();
    }

    AddedInputMappingMap.Reset();
}

void UGameFeatureAction_AddInputContextMapping::OnGameFeatureUnregistering()
{
    Super::OnGameFeatureUnregistering();
}

void UGameFeatureAction_AddInputContextMapping::AddInputMappingForPlayer(APawn* InPawn)
{
    APlayerController* PC = Cast<APlayerController>(InPawn->GetController());
    if (!PC)
    {
        return;
    }
    
    ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PC->GetLocalPlayer());
    if (!LocalPlayer)
    {
        return;
    }
    
    UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
    if (!InputSubsystem)
    {
        return;
    }

    TArray<UInputMappingContext*>& InputMappingContextArray = AddedInputMappingMap.FindOrAdd(InPawn);
    for (FInputMappingSet InputMappingSet : InputMapping.GrantInputMappingArray)
    {
        if (IsValid(LocalPlayer))
        {
            UInputMappingContext* AddIMC = InputMappingSet.InputMappingContext.LoadSynchronous();
            InputSubsystem->AddMappingContext(AddIMC, InputMappingSet.Priority);
            InputMappingContextArray.Add(AddIMC);
            
            UE_LOG(LogBS, Warning, TEXT("UGameFeatureAction_AddInputContextMapping::InputMappingContext %s 부여 성공"), *AddIMC->GetName());
        }
    }
}

void UGameFeatureAction_AddInputContextMapping::RemoveInputMapping(APawn* InPawn)
{
    APlayerController* PC = Cast<APlayerController>(InPawn->GetController());
    if (!PC)
    {
        return;
    }
    
    ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(PC->GetLocalPlayer());
    if (!LocalPlayer)
    {
        return;
    }
    
    UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
    if (!InputSubsystem)
    {
        return;
    }

    InputSubsystem->GetPlayerInput()->FlushPressedKeys();

    auto InputMappingContextArray = AddedInputMappingMap.Find(InPawn);
    for (const UInputMappingContext* InputMappingContext : *InputMappingContextArray)
    {
        InputSubsystem->RemoveMappingContext(InputMappingContext);
        
        UE_LOG(LogBS, Warning, TEXT("UGameFeatureAction_AddInputContextMapping::액터 %s 의 IMC %s 제거 완료"), *InPawn->GetName(), *InputMappingContext->GetName());
    }

    AddedInputMappingMap.Remove(InPawn);
}
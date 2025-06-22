// ============================================================================
// 3. TestComponentC.h - 세 번째로 초기화되는 컴포넌트 (A, B에 의존)
// ============================================================================
#pragma once

#include "CoreMinimal.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "GameplayTagContainer.h"
#include "TestComponentC.generated.h"

UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class BISHOUJO_DOOM_API UTestComponentC : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	UTestComponentC(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// IGameFrameworkInitStateInterface
	virtual FName GetFeatureName() const override { return NAME_TestComponentC; }
	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
	virtual void CheckDefaultInitialization() override;

protected:
	virtual void OnRegister() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	static const FName NAME_TestComponentC;

	UPROPERTY()
	bool bResourcesLoaded = false;

	void LoadResources();
	void FinalizeSetup();
};
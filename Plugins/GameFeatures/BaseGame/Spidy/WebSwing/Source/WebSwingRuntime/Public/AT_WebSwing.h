// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_WebSwing.generated.h"

/**
 * 
 */
UCLASS()
class WEBSWINGRUNTIME_API UAT_WebSwing : public UAbilityTask
{
	GENERATED_BODY()

public:
	UFUNCTION()
	static UAT_WebSwing* CreateWebSwingTask(UGameplayAbility* InOwningAbility, const FVector& InAttachPoint);
	
protected:
	virtual void Activate() override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

public:
	FSimpleMulticastDelegate OnFinished;

private:
	// 로프 방향 힘 (매달림)
	FVector TargetAttachPoint;		//원의 중심
	float WebLength;				//원의 반지름
	float CalculatedWebTension;		//웹의 장력

	// 접선 방향 힘 (가속)
	FVector CurrentSwingVelocity;	//매 프레임 스윙 순간 속도
	FVector TangentForceDirection;	//접선 방향, 순간 방향
	float CurrentSwingAngle;		//현재 스윙 각도(로프 대비)
};

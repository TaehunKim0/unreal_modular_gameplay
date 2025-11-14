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
	virtual void TickTask(float DeltaTime) override;
	virtual void OnDestroy(bool bInOwnerFinished) override;

	void UpdateCharacterRotation(float DeltaTime);
	void UpdateKeyboardInput(const FVector2D& MovementValue);

	FRotator CalculateSwingSideAngle();
	FVector CalculateStringTension(float InVelocityClampMin, float InVelocityClampMax) const;
	FVector IncreaseSpeedAtBottomOfArc();

public:
	FSimpleMulticastDelegate OnFinished;

private:
	// 로프 방향 힘 (매달림)
	FVector TargetAttachPoint;		//원의 중심
	float Radius;				//원의 반지름

	float AngularSpeed = 2.0f;
	float Mass = 20.f;
	bool bInitialVelocitySet = false;
};

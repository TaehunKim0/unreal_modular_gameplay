// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CentripetalTestActor.generated.h"

UCLASS()
class WEBSWINGRUNTIME_API ACentripetalTestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ACentripetalTestActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	FVector AttachPoint; // 원의 중심 (웹이 붙은 지점)
	float Radius; // 원의 반지름 (웹 길이 L)
	float AngularSpeed = 1.0f; // 각속도 (초당 라디안) - 속도 조절
	bool bInitialVelocitySet = false;

private:
	float CurrentAngle = 0.0f; // 현재 회전 각도 (0~2*PI)
};

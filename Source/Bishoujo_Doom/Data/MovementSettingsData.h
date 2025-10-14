// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MovementSettingsData.generated.h"

/**
 * Movement 속도 설정
 */
USTRUCT()
struct FMovementSpeedSettings
{
    GENERATED_BODY()

    /** 기본 걷기 속도 */
    UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
    float WalkSpeed = 400.0f;

    /** 달리기 속도 */
    UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0"))
    float SprintSpeed = 600.0f;

    /** 뒤로 걷기 속도 배율 */
    UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float BackwardSpeedMultiplier = 0.7f;

    /** 옆으로 걷기 속도 배율 */
    UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float StrafeSpeedMultiplier = 0.85f;
};

/**
 * Movement 가속도 설정
 */
USTRUCT()
struct FMovementAccelerationSettings
{
    GENERATED_BODY()

    /** 최대 가속도 */
    UPROPERTY(EditAnywhere)
    float MaxAcceleration = 2048.0f;

    /** 감속도 (브레이크) */
    UPROPERTY(EditAnywhere)
    float BrakingDeceleration = 2048.0f;

    /** 마찰력 */
    UPROPERTY(EditAnywhere)
    float GroundFriction = 8.0f;
};

/**
 * Movement 회전 설정
 */
USTRUCT()
struct FMovementRotationSettings
{
    GENERATED_BODY()

    /** 회전 속도 (도/초) */
    UPROPERTY(EditAnywhere)
    float RotationRate = 540.0f;

    /** 이동 방향으로 자동 회전 */
    UPROPERTY(EditAnywhere)
    bool bOrientRotationToMovement = true;

    /** 컨트롤러 방향으로 회전 (3인칭에서 false, 1인칭에서 true) */
    UPROPERTY(EditAnywhere)
    bool bUseControllerDesiredRotation = false;
};

/**
 * Sprint 설정
 */
USTRUCT()
struct FSprintSettings
{
    GENERATED_BODY()

    /** Sprint 시 스태미나 소모 (초당) */
    UPROPERTY(EditAnywhere)
    float StaminaCostPerSecond = 5.0f;

    /** Sprint 시 스태미나 자동 회복 중지 */
    UPROPERTY(EditAnywhere)
    bool bStopStaminaRegenWhileSprinting = true;

    /** Sprint를 중지할 스태미나 임계값 (%) */
    UPROPERTY(EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "100.0"))
    float StopStaminaThreshold = 10.0f;
};

USTRUCT(BlueprintType)
struct FMovementSettingsData : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "Settings")
    FMovementSpeedSettings SpeedSettings;

    UPROPERTY(EditAnywhere, Category = "Settings")
    FMovementAccelerationSettings AccelerationSettings;

    UPROPERTY(EditAnywhere, Category = "Settings")
    FMovementRotationSettings RotationSettings;

    UPROPERTY(EditAnywhere, Category = "Settings")
    FSprintSettings SprintSettings;
};
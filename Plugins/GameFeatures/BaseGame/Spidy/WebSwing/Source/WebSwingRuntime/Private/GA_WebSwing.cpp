// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_WebSwing.h"

#include "BSLogChannels.h"
#include "GameFramework/Character.h"

UGA_WebSwing::UGA_WebSwing(const FObjectInitializer& ObjectInitializer)
{
}

void UGA_WebSwing::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FHitResult WebHitResult;
	LaunchWebToMouse(WebHitResult);

	if (WebHitResult.bBlockingHit)
	{
		FVector AttachPoint = WebHitResult.ImpactPoint;
        
		// 스윙 모드 vs 집 모드 판단
		if (CanUseSwingMode(AttachPoint))
		{
			UE_LOG(LogBS, Log, TEXT("Web Swing"));
			ExecuteWebSwing(AttachPoint);
		}
		else
		{
			UE_LOG(LogBS, Log, TEXT("Web Zip"));
			ExecuteWebZip(AttachPoint);
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		}
	}
	else
	{
		// 웹이 닿지 않으면 어빌리티 종료
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
}

void UGA_WebSwing::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UGA_WebSwing::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UGA_WebSwing::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	EndAbility(Handle,ActorInfo,ActivationInfo,true, false);
}

void UGA_WebSwing::LaunchWebToMouse(FHitResult& OutHitResult)
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(Character->GetController());
	if (!PC)
	{
		return;
	}

	// 카메라 위치와 방향 가져오기
	FVector CameraLocation;
	FRotator CameraRotation;
	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

	// Line Trace 설정
	FVector TraceStart = CameraLocation;
	FVector TraceEnd = CameraLocation + (CameraRotation.Vector() * MaxWebDistance);

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Character);
	QueryParams.bTraceComplex = true;

	// Line Trace 실행
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		WebTraceChannel,
		QueryParams
	);
	
	if (bHit)
	{
		OutHitResult = HitResult;
	}
	
	// 디버그 라인 (개발 중)
#if WITH_EDITOR
	const bool bShowDebugLine = true;
	if (bShowDebugLine)
	{
		DrawDebugLine(
			GetWorld(),
			TraceStart,
			bHit ? HitResult.ImpactPoint : TraceEnd,
			bHit ? FColor::Green : FColor::Red,
			false,
			2.0f,
			0,
			2.0f
		);

		if (bHit)
		{
			DrawDebugSphere(
				GetWorld(),
				HitResult.ImpactPoint,
				20.0f,
				12,
				FColor::Yellow,
				false,
				2.0f
			);
		}
	}
#endif
}

void UGA_WebSwing::ExecuteWebSwing(FVector InAttachPoint)
{
}

void UGA_WebSwing::ExecuteWebZip(FVector InAttachPoint)
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		return;
	}

	UCharacterMovementComponent* CharacterMovement = Character->GetCharacterMovement();
	if (!CharacterMovement)
	{
		return;
	}

	// 목표 지점으로의 방향과 거리 계산
	FVector CharacterLocation = Character->GetActorLocation();
	FVector DirectionToTarget = (InAttachPoint - CharacterLocation).GetSafeNormal();
	float DistanceToTarget = FVector::Dist(CharacterLocation, InAttachPoint);

	// Launch Velocity 계산
	FVector LaunchVelocity = DirectionToTarget * ZipSpeed;

	// 캐릭터 발사
	Character->LaunchCharacter(LaunchVelocity, true, true);

	// // 착지 감지를 위한 AbilityTask 생성
	// UAbilityTask_WaitMovementModeChange* LandTask = UAbilityTask_WaitMovementModeChange::CreateWaitMovementModeChange(
	// 	this,
	// 	EMovementMode::MOVE_Falling
	// );
	//
	// if (LandTask)
	// {
	// 	LandTask->OnChange.AddDynamic(this, &UGA_WebSwing::OnWebZipLanded);
	// 	LandTask->ReadyForActivation();
	// }

	// // 타임아웃 설정 (만약을 위해)
	// FTimerHandle TimeoutHandle;
	// GetWorld()->GetTimerManager().SetTimer(
	// 	TimeoutHandle,
	// 	[this]()
	// 	{
	// 		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	// 	},
	// 	5.0f, // 5초 후 강제 종료
	// 	false
	// );
}

bool UGA_WebSwing::CanUseSwingMode(FVector InAttachPoint)
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		return false;
	}

	// 캐릭터에서 부착점으로의 방향 벡터
	FVector CharacterLocation = Character->GetActorLocation();
	FVector ToAttachPoint = (InAttachPoint - CharacterLocation).GetSafeNormal();

	// 수평면 기준 각도 계산 (Z 성분으로 판단)
	// Z > 0: 위쪽, Z < 0: 아래쪽
	float AngleToHorizon = FMath::RadiansToDegrees(FMath::Asin(ToAttachPoint.Z));

	// 임계값보다 위쪽이면 Swing 모드
	// 예: 45도 이상이면 Swing, 미만이면 Zip
	return AngleToHorizon > SwingAngleThreshold;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_WebSwing.h"

#include "AT_WebSwing.h"
#include "BSLogChannels.h"
#include "CentripetalTestActor.h"
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
			ExecuteWebSwing(AttachPoint);
		}
		else
		{
			ExecuteWebZip(AttachPoint);
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		}
	}
	else
	{
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

void UGA_WebSwing::ExecuteWebSwing(const FVector& InAttachPoint)
{
	//UAT_WebSwing* WebSwingTask = UAT_WebSwing::CreateWebSwingTask(this, InAttachPoint);

	// if (WebSwingTask)
	// {
	// 	WebSwingTask->OnFinished.AddUObject(this, &ThisClass::OnWebSwingFinished);
	// 	WebSwingTask->ReadyForActivation();
	// }

	// 1. 스폰 위치 설정 (캐릭터 위치)
	FVector SpawnLocation = GetAvatarActorFromActorInfo()->GetActorLocation();

	// 2. 스폰 파라미터 설정
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetAvatarActorFromActorInfo();

	// 3. 🌟 테스트 액터 스폰 🌟
	auto ResultActor = GetWorld()->SpawnActor<ACentripetalTestActor>(TestActor.LoadSynchronous(), SpawnLocation, FRotator::ZeroRotator, SpawnParams);

	if (ResultActor)
	{
		// 4. 테스트 액터에 핵심 정보 전달
		ResultActor->AttachPoint = InAttachPoint; // 웹이 붙은 지점을 원의 중심으로 설정
		ResultActor->Radius = FVector::Dist(InAttachPoint, SpawnLocation); // 현재 거리를 반지름으로 설정
		ResultActor->AngularSpeed = 1.5f; // 원하는 속도로 회전 시작
	}
}

void UGA_WebSwing::ExecuteWebZip(const FVector& InAttachPoint) const
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

	const FVector CharacterLocation = Character->GetActorLocation();
	const FVector DirectionToTarget = (InAttachPoint - CharacterLocation).GetSafeNormal();

	FVector LaunchVelocity = DirectionToTarget * ZipSpeed;
	LaunchVelocity.Z += ZipUpwardBoost;

	Character->LaunchCharacter(LaunchVelocity, true, true);
}

bool UGA_WebSwing::CanUseSwingMode(const FVector& InAttachPoint) const
{
	ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Character)
	{
		return false;
	}

	FVector CharacterLocation = Character->GetActorLocation();
	if (CharacterLocation.Z > InAttachPoint.Z)
	{
		return false;
	}

	return true;
}

void UGA_WebSwing::OnWebSwingFinished()
{
	K2_EndAbility();
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "AT_WebSwing.h"

#include "BSLogChannels.h"
#include "GA_WebSwing.h"
#include "Character/BSCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

UAT_WebSwing* UAT_WebSwing::CreateWebSwingTask(UGameplayAbility* InOwningAbility, const FVector& InAttachPoint)
{
	UAT_WebSwing* MyTask = NewAbilityTask<UAT_WebSwing>(InOwningAbility);

	if (MyTask)
	{
		MyTask->bTickingTask = true;
		MyTask->TargetAttachPoint = InAttachPoint;
	}
    
	return MyTask;
}

void UAT_WebSwing::Activate()
{
	Super::Activate();
}

void UAT_WebSwing::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	auto AvatarActor = GetGameplayTaskAvatar(this);
	auto MovementComp = Cast<ACharacter>(AvatarActor)->GetCharacterMovement(); 
	auto CameraBoom = Cast<ABSCharacter>(AvatarActor)->CameraBoom;
	auto CameraDirection = CameraBoom->GetForwardVector();
	CameraDirection.Z = 0.0f;

	// 1. Forward Pushing Force
	FVector ForwardPushingForce = CameraDirection * 130000.f;

	// 2. Tension Force
	FVector TensionForce = CalculateStringTension(100,3000);

	// 3. Increase Swing Speed it reached at the Bottom of the Arc
	FVector IncreasedSpeed = IncreaseSpeedAtBottomOfArc();
	
	// Result Force
	FVector ResultForce = ForwardPushingForce + TensionForce + IncreasedSpeed;
	MovementComp->AddForce(ResultForce);

	DrawDebugDirectionalArrow(
		GetWorld(),
		AvatarActor->GetActorLocation(),
		AvatarActor->GetActorLocation() + MovementComp->Velocity.GetSafeNormal() * 200.f,
		5.f,
		FColor::Blue,
		true,
		0.0f,
		0,
		2.f
	);

	UpdateCharacterRotation(DeltaTime);

	UE_LOG(LogBS, Log, TEXT("IncreasedSpeed : %f"), IncreasedSpeed.Size());
}

void UAT_WebSwing::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);

	UE_LOG(LogBS, Log, TEXT("OnDestroy"));
}

void UAT_WebSwing::UpdateCharacterRotation(float DeltaTime)
{
	auto AvatarActor = GetGameplayTaskAvatar(this);
	auto Character = Cast<ACharacter>(AvatarActor);
	if (!Character) return;

	auto MovementComp = Character->GetCharacterMovement();
	FVector CurrentVelocity = MovementComp->Velocity;

	auto SwingAngle = CalculateSwingSideAngle();
	FRotator SwingRotation = FRotator(SwingAngle.Pitch,CurrentVelocity.Rotation().Yaw, SwingAngle.Roll);

	auto ResultRotation = FMath::RInterpTo(Character->GetActorRotation(), SwingRotation, DeltaTime, 20.f);
	Character->SetActorRotation(ResultRotation);
}

FRotator UAT_WebSwing::CalculateSwingSideAngle()
{
	auto AvatarActor = GetGameplayTaskAvatar(this);
	auto MovementComp = Cast<ACharacter>(AvatarActor)->GetCharacterMovement();

	FVector ToAttachPointDir = (TargetAttachPoint - AvatarActor->GetActorLocation()).GetSafeNormal();
	FVector StandardAxis = FVector::CrossProduct(ToAttachPointDir, MovementComp->Velocity.GetSafeNormal());

	auto Result = FRotationMatrix::MakeFromZY(ToAttachPointDir,StandardAxis);

	return FRotator(Result.Rotator());
}

FVector UAT_WebSwing::CalculateStringTension(float InVelocityClampMin, float InVelocityClampMax) const
{
	auto MovementComp = Cast<ACharacter>(GetAvatarActor())->GetMovementComponent();
	FVector CurrentVelocity = MovementComp->Velocity.GetClampedToSize(InVelocityClampMin, InVelocityClampMax);
	FVector ToCharacterDirection = GetAvatarActor()->GetActorLocation() - TargetAttachPoint;

	float Dot = FVector::DotProduct(CurrentVelocity, ToCharacterDirection);

	return ToCharacterDirection.GetSafeNormal() * Dot * -1.f; //반대 방향으로 힘 주기
}

// FVector UAT_WebSwing::GetCurrentSwingAngle() const
// {
// 	FVector ToAttachPointDir = (TargetAttachPoint - GetAvatarActor()->GetActorLocation()).GetSafeNormal();	
// 	auto MovementComp = Cast<ACharacter>(GetAvatarActor())->GetMovementComponent();
// 	FVector CurrentVelocity = MovementComp->Velocity;
//
// 	FVector ResultVector = FVector::CrossProduct(CurrentVelocity, ToAttachPointDir);
//
// 	FRotationMatrix::MakeFromZY(ToAttachPointDir, ResultVector);
// 	
// 	return ResultVector;
// }

FVector UAT_WebSwing::IncreaseSpeedAtBottomOfArc()
{
	FVector ToAttachDir = (TargetAttachPoint - GetAvatarActor()->GetActorLocation()).GetSafeNormal();
	auto Dot = FVector::DotProduct(FVector(0,0,1), ToAttachDir);
	
	if (Dot > 0.8f)
	{
		auto MovementComp = Cast<ACharacter>(GetAvatarActor())->GetMovementComponent();
		FVector CurrentVelocity = MovementComp->Velocity;

		float Power = 25000.f;
		FVector ResultSpeed = CurrentVelocity.GetSafeNormal() * Power;

		return ResultSpeed;
	}

	return FVector::ZeroVector;
}

/*
*	// 새로운 방향 계산 (현재 위치에서의 접선 방향)
	FVector CurrentToTarget = (TargetAttachPoint - AvatarActor->GetActorLocation()).GetSafeNormal();
	FVector NewTangentDirection = FVector::CrossProduct(CurrentToTarget, FVector::UpVector).GetSafeNormal();
	DrawDebugDirectionalArrow(
			GetWorld(),
			AvatarActor->GetActorLocation(),                    // 시작점
			AvatarActor->GetActorLocation() + NewTangentDirection * 200.f,  // 끝점 (200은 화살표 길이)
			5.f,                                                 // 화살표 크기
			FColor::Blue,                                       // 색상
			false,                                               // 지속 여부
			1.0f,                                               // 지속 시간 (0 = 한 프레임)
			0,                                                   // Depth Priority
			2.f                                                  // 선 두께
		);
 void UAT_WebSwing::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	// 현재 위치에서 중심점까지의 벡터
	auto AvatarActor = GetGameplayTaskAvatar(this);
	FVector ToTargetDirection = (TargetAttachPoint - AvatarActor->GetActorLocation()).GetSafeNormal();

	auto MovementComp = Cast<ACharacter>(AvatarActor)->GetMovementComponent(); 

	if (!bInitialVelocitySet)
	{
		//* 1. 초기 접선 속도 구하기*
		// 접선 방향 = 중심 방향과 수직인 방향 (외적으로 계산)
		FVector TangentDirection = FVector::CrossProduct(ToTargetDirection, FVector::UpVector).GetSafeNormal();
		
		DrawDebugDirectionalArrow(
			GetWorld(),
			AvatarActor->GetActorLocation(),                    // 시작점
			AvatarActor->GetActorLocation() + TangentDirection * 200.f,  // 끝점 (200은 화살표 길이)
			5.f,                                                 // 화살표 크기
			FColor::Green,                                       // 색상
			false,                                               // 지속 여부
			5.0f,                                               // 지속 시간 (0 = 한 프레임)
			0,                                                   // Depth Priority
			2.f                                                  // 선 두께
		);

		// 각속도 (단위 시간 동안 회전하는 각도) w
		AngularSpeed = 2.f;
    	
		// 속도 (단위 시간 동안 변화한 변위) v = r * w
		float InitialSpeed = Radius * AngularSpeed;

		// 초기 속도 적용 (접선 방향으로 날아가게)
		FVector InitialVelocity = TangentDirection * InitialSpeed;
		MovementComp->Velocity = InitialVelocity;
        
		bInitialVelocitySet = true;
	}

	//* 2. 구심력 구하기 *
	// 현재 속도 v
	FVector CurrentVelocity = MovementComp->Velocity;
	float CurrentSpeed = CurrentVelocity.Size();
    
	// 구심 가속도 a = v^2 / r
	float CentripetalAccelerationMagnitude = (CurrentSpeed * CurrentSpeed) / Radius;
	
	// 구심력 = m * a = m * 구심 가속도
	float CentripetalForceMagnitude = 60 * CentripetalAccelerationMagnitude; // F = m * a
    
	// 최종 힘 : 구심력 * 방향
	FVector DirectionUnitVector = (TargetAttachPoint - AvatarActor->GetActorLocation()).GetSafeNormal();
	
	// 새로운 방향 계산 (현재 위치에서의 접선 방향)
	FVector CurrentToTarget = (TargetAttachPoint - AvatarActor->GetActorLocation()).GetSafeNormal();
	FVector NewTangentDirection = FVector::CrossProduct(CurrentToTarget, FVector::UpVector).GetSafeNormal();
	DrawDebugDirectionalArrow(
			GetWorld(),
			AvatarActor->GetActorLocation(),                    // 시작점
			AvatarActor->GetActorLocation() + NewTangentDirection * 200.f,  // 끝점 (200은 화살표 길이)
			5.f,                                                 // 화살표 크기
			FColor::Blue,                                       // 색상
			false,                                               // 지속 여부
			1.0f,                                               // 지속 시간 (0 = 한 프레임)
			0,                                                   // Depth Priority
			2.f                                                  // 선 두께
		);

	// 속도를 새 접선 방향으로 설정 (크기는 유지)
	MovementComp->Velocity = NewTangentDirection * CurrentSpeed;
}
 */

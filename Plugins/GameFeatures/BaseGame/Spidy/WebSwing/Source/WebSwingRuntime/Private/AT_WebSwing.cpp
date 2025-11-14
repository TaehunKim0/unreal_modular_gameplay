// Fill out your copyright notice in the Description page of Project Settings.


#include "AT_WebSwing.h"

#include "Etc/BSLogChannels.h"
#include "GA_WebSwing.h"
#include "Character/BSCharacter.h"
#include "Character/Component/BSPawnInputComponent.h"
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

	auto AvatarActor = GetGameplayTaskAvatar(this);
	auto BSCharacter = Cast<ABSCharacter>(AvatarActor);
	if (BSCharacter && BSCharacter->PawnInputComponent)
	{
		BSCharacter->PawnInputComponent->OnPawnMoveDelegate.AddUObject(this, &UAT_WebSwing::UpdateKeyboardInput);
	}
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
	
	UpdateCharacterRotation(DeltaTime);
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

void UAT_WebSwing::UpdateKeyboardInput(const FVector2D& MovementValue)
{
	auto AvatarActor = GetGameplayTaskAvatar(this);
	auto Character = Cast<ACharacter>(AvatarActor);
	if (!Character) return;

	if (MovementValue.X != 0.0f)
	{
		auto RightDir = Character->GetActorRightVector().GetSafeNormal();
	
		float Power = 110000.f;
		FVector Force = RightDir * MovementValue.X * Power;
	
		Character->GetCharacterMovement()->AddForce(Force);
	}
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
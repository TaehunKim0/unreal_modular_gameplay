// Fill out your copyright notice in the Description page of Project Settings.


#include "CentripetalTestActor.h"

#include "Etc/BSLogChannels.h"

// Sets default values
ACentripetalTestActor::ACentripetalTestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 🌟 1. 물리 컴포넌트 가져오기 (예: 루트 컴포넌트)
	UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(GetRootComponent());

	if (PrimitiveComp)
	{
		// 🌟 2. 물리 시뮬레이션 활성화
		PrimitiveComp->SetSimulatePhysics(true);
        
		// 🌟 3. 등속 원운동 테스트를 위해 중력 비활성화 (선택 사항)
		PrimitiveComp->SetEnableGravity(false); 
        
		// 질량을 계산하려면 SetSimulatePhysics(true) 이전에 호출되어야 함
		// PrimitiveComp->SetMassOverrideInKg(NAME_None, 80.0f, true); 
	}
}

// Called when the game starts or when spawned
void ACentripetalTestActor::BeginPlay()
{
	Super::BeginPlay();
}

void ACentripetalTestActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    UPrimitiveComponent* PrimitiveComp = Cast<UPrimitiveComponent>(GetRootComponent());
    if (!PrimitiveComp || !PrimitiveComp->IsSimulatingPhysics())
    {
       UE_LOG(LogBS, Warning, TEXT("CentripetalTestActor is not simulating physics."));
       return;
    }
    
    float Mass = 80.0f; // 캐릭터 질량 (m)
	Radius; // 반지름 (r)

    // 현재 위치에서 중심점까지의 벡터
    FVector ToCenter = AttachPoint - GetActorLocation();
    FVector DirectionToCenter = ToCenter.GetSafeNormal();

    if (!bInitialVelocitySet)
    {
//* 1. 초기 접선 속도 구하기*//
       // 접선 방향 = 중심 방향과 수직인 방향 (외적으로 계산)
       FVector TangentDirection = FVector::CrossProduct(DirectionToCenter, FVector::UpVector).GetSafeNormal();

       // 각속도 (단위 시간 동안 회전하는 각도) w
       AngularSpeed = 3.0f;
    	
       // 속도 (단위 시간 동안 변화한 변위) v = r * w
       float InitialSpeed = Radius * AngularSpeed;

       // 초기 속도 적용 (접선 방향으로 날아가게)
       FVector InitialVelocity = TangentDirection * InitialSpeed;
       PrimitiveComp->SetPhysicsLinearVelocity(InitialVelocity);
        
       bInitialVelocitySet = true;
       UE_LOG(LogTemp, Log, TEXT("Initial tangent velocity set: %s"), *InitialVelocity.ToString());
    }

//* 2. 구심력 구하기 *//
    // 현재 속도 v
    FVector CurrentVelocity = PrimitiveComp->GetPhysicsLinearVelocity();
    float CurrentSpeed = CurrentVelocity.Size();
    
    // 구심 가속도 a = v^2 / r
    float CentripetalAccelerationMagnitude = (CurrentSpeed * CurrentSpeed) / Radius;

    // 구심력 = m * a = m * 구심 가속도
    float CentripetalForceMagnitude = Mass * CentripetalAccelerationMagnitude; // F = m * a
    
    // 최종 힘 : 구심력 * 방향
    FVector DirectionUnitVector = (AttachPoint - GetActorTransform().GetLocation()).GetSafeNormal();
    FVector Force = DirectionUnitVector * CentripetalForceMagnitude;

    PrimitiveComp->AddForce(Force);
}

// 진자 운동
// 속력, 운동 방향 모두 바뀜
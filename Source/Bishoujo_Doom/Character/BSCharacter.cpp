// Fill out your copyright notice in the Description page of Project Settings.


#include "BSCharacter.h"

#include "BSLogChannels.h"
#include "AbilitySystem/BSAbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Component/BSPawnExtensionComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "TestComponent/TestComponentA.h"
#include "TestComponent/TestComponentB.h"
#include "TestComponent/TestComponentC.h"

// Sets default values
ABSCharacter::ABSCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 캐릭터 충돌 캡슐 크기 설정
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// 컨트롤러가 회전할 때 캐릭터가 같이 회전하지 않도록 설정
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	PawnExtComponent = CreateDefaultSubobject<UBSPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	//PawnExtComponent->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
	//PawnExtComponent->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));

	TestComponentA = CreateDefaultSubobject<UTestComponentA>(TEXT("TestComponentA"));
	TestComponentB = CreateDefaultSubobject<UTestComponentB>(TEXT("TestComponentB"));
	TestComponentC = CreateDefaultSubobject<UTestComponentC>(TEXT("TestComponentC"));
	
	// 캐릭터 이동 설정
	GetCharacterMovement()->bOrientRotationToMovement = true; // 캐릭터가 이동 방향으로 회전
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // 회전 속도
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// 카메라 붐 생성 (캐릭터 뒤에서 카메라를 당겨옴)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // 카메라와 캐릭터 사이의 거리
	CameraBoom->bUsePawnControlRotation = true; // 컨트롤러 입력에 따라 팔 회전

	// 팔로우 카메라 생성
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // 카메라는 붐에 대해 상대적으로 회전하지 않음
	
}

void ABSCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	UE_LOG(LogBS, Log, TEXT("ABSCharacter::PreInitializeComponents"));
}

void ABSCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	UE_LOG(LogBS, Log, TEXT("ABSCharacter::PossessedBy"));
}

// Called when the game starts or when spawned
void ABSCharacter::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogBS, Log, TEXT("ABSCharacter::BeginPlay"));
}

void ABSCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void ABSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

UBSAbilitySystemComponent* ABSCharacter::GetBSAbilitySystemComponent() const
{
	return Cast<UBSAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* ABSCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


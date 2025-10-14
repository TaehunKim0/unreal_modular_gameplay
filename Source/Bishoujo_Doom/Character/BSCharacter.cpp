// Fill out your copyright notice in the Description page of Project Settings.


#include "BSCharacter.h"

#include "BSLogChannels.h"
#include "AbilitySystem/BSAbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Component/BSCharacterMovementComponent.h"
#include "Component/BSDefaultCharacterComponent.h"
#include "Component/BSPawnExtensionComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/BSPlayerState.h"
#include "UI/SubSystem/BSPlayerUISubSystem.h"

// Sets default values
ABSCharacter::ABSCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UBSCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// 컨트롤러가 회전할 때 캐릭터가 같이 회전하지 않도록 설정
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	DefaultCharacterComponent = CreateDefaultSubobject<UBSDefaultCharacterComponent>(TEXT("DefaultCharacterComponent"));
	PawnExtComponent = CreateDefaultSubobject<UBSPawnExtensionComponent>(TEXT("PawnExtensionComponent"));
	//PawnExtComponent->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
	//PawnExtComponent->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));

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
	
	const auto BSPlayerState = Cast<ABSPlayerState>(GetPlayerState());
	if (!IsValid(BSPlayerState))
	{
		UE_LOG(LogBS, Error, TEXT("ABSCharacter::BSPlayerState is not valid"));
		return;
	}

	AbilitySystemComponent = BSPlayerState->GetBSAbilitySystemComponent();
	AbilitySystemComponent->InitAbilityActorInfo(BSPlayerState, this);

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

	UE_LOG(LogBS, Log, TEXT("ABSCharacter::EndPlay"));
}

// Called every frame
void ABSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	auto Result = GetBSAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Ability.Action.Jump"));
	FString ResultString = Result ? TEXT("True") : TEXT("False");

	FString IsGroundedString = GetMovementComponent()->IsMovingOnGround() ? TEXT("True") : TEXT("False");
	
	UBSPlayerUISubSystem::Get(this)->ShowDebugMessage(TEXT("Ability.Jump"), ResultString);
	UBSPlayerUISubSystem::Get(this)->ShowDebugMessage(TEXT("Grounded"), IsGroundedString);
}

UBSAbilitySystemComponent* ABSCharacter::GetBSAbilitySystemComponent() const
{
	return Cast<UBSAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* ABSCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


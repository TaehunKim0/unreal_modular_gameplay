// Fill out your copyright notice in the Description page of Project Settings.


#include "BSCharacter.h"

#include <string>

#include "BSGamePlayTags.h"
#include "BSLogChannels.h"
#include "EnhancedActionKeyMapping.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "AbilitySystem/BSAbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "Component/BSCharacterMovementComponent.h"
#include "Component/BSPawnInputComponent.h"
#include "Component/BSHealthComponent.h"
#include "Component/BSPawnStateManagerComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Input/BSInputComponent.h"
#include "Player/BSPlayerState.h"
#include "UI/SubSystem/BSPlayerUISubSystem.h"

ABSCharacter::ABSCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UBSCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// 컨트롤러가 회전할 때 캐릭터가 같이 회전하지 않도록 설정
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	PawnStateManagerComponent = CreateDefaultSubobject<UBSPawnStateManagerComponent>(TEXT("PawnStateManagerComponent"));
	PawnInputComponent = CreateDefaultSubobject<UBSPawnInputComponent>(TEXT("PawnInputComponent"));
	HealthComponent = CreateDefaultSubobject<UBSHealthComponent>(TEXT("HealthComponent"));

	// 카메라 붐 생성 (캐릭터 뒤에서 카메라를 당겨옴)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // 카메라와 캐릭터 사이의 거리
	CameraBoom->bUsePawnControlRotation = true; // 컨트롤러 입력에 따라 팔 회전

	// 팔로우 카메라 생성
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false; // 카메라는 붐에 대해 상대적으로 회전하지 않음

	ConstructorHelpers::FClassFinder<UUserWidget> FindWidget(TEXT("/Game/Bishoujo_Doom/UI/WB_DebugWidget.WB_DebugWidget_C"));
	if (FindWidget.Succeeded())
	{
		DebugWidgetClass = FindWidget.Class;
	}

	PawnStateManagerComponent->OnPawnGameplayReadyCompleted.AddUObject(this, &ABSCharacter::OnPawnGameplayReadyComplete);
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
		UE_LOG(LogBS, Error, TEXT("ABSCharacter::BSPlayerState is not Valid"));
		return;
	}

	AbilitySystemComponent = BSPlayerState->GetBSAbilitySystemComponent();
	AbilitySystemComponent->InitAbilityActorInfo(BSPlayerState, this);

	UE_LOG(LogBS, Log, TEXT("ABSCharacter::PossessedBy"));
	OnPossessedDelegate.Broadcast(this);
}

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

void ABSCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	OnRepPlayerStateDelegate.Broadcast(GetPlayerState());
}

void ABSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PawnStateManagerComponent->GetInitState() != BSGamePlayTags::InitState_GameplayReady)
	{
		return;
	}

	FString SHealth; SHealth.AppendInt(HealthComponent->GetHealth());
	UBSPlayerUISubSystem::Get(this)->ShowDebugMessage("Health", SHealth );
}

UBSAbilitySystemComponent* ABSCharacter::GetBSAbilitySystemComponent() const
{
	return Cast<UBSAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* ABSCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ABSCharacter::OnPawnGameplayReadyComplete()
{
	UBSPlayerUISubSystem::Get(this)->CreateWidget<UBSDebugWidget>(DebugWidgetClass, EUICategory::Debug, GetPlayerState()->GetPlayerController());
	
	const auto BSPlayerState = Cast<ABSPlayerState>(GetPlayerState());
	UBSPlayerUISubSystem::Get(this)->ShowPawnAbilitySetMessage(BSPlayerState, BSPlayerState->GetCharacterDefData());
}
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ModularCharacter.h"
#include "GameFramework/Character.h"
#include "BSCharacter.generated.h"

class UBSHealthComponent;
class UInputMappingContext;
class UBSPawnInputComponent;
class UBSDebugWidget;
class ABSHUD;
class UBSPawnStateManagerComponent;
class UBSAbilitySystemComponent;
class USpringArmComponent;
class UCameraComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnpossessDelegate, APawn*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRepPlayerStateDelegate, APlayerState*);

UCLASS()
class BISHOUJO_DOOM_API ABSCharacter : public AModularCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ABSCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
	virtual void PreInitializeComponents() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnRep_PlayerState() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "BS|Character")
	UBSAbilitySystemComponent* GetBSAbilitySystemComponent() const;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:
	UFUNCTION()
	void OnPawnGameplayReadyComplete();

public:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BS|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBSPawnStateManagerComponent> PawnStateManagerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BS|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBSPawnInputComponent> PawnInputComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BS|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBSHealthComponent> HealthComponent;

public:
	FOnpossessDelegate OnPossessedDelegate;
	FOnRepPlayerStateDelegate OnRepPlayerStateDelegate;

private:
	TArray<UInputMappingContext> InputMappingContexts;
	TSubclassOf<UBSDebugWidget> DebugWidgetClass;
};
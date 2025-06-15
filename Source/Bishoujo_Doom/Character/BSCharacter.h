// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "BSCharacter.generated.h"

class UBSAbilitySystemComponent;
class USpringArmComponent;
class UCameraComponent;

UCLASS()
class BISHOUJO_DOOM_API ABSCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ABSCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void PreInitializeComponents() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Lyra|Character")
	UBSAbilitySystemComponent* GetBSAbilitySystemComponent() const;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

public:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** Pointer to the ability system component that is cached for convenience. */
	UPROPERTY()
	TObjectPtr<UBSAbilitySystemComponent> AbilitySystemComponent;
};
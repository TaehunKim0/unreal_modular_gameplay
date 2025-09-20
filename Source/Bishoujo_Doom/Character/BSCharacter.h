// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ModularCharacter.h"
#include "GameFramework/Character.h"
#include "BSCharacter.generated.h"

class UBSPawnExtensionComponent;
class UBSAbilitySystemComponent;
class USpringArmComponent;
class UCameraComponent;
class UTestComponentA;
class UTestComponentB;
class UTestComponentC;

UCLASS()
class BISHOUJO_DOOM_API ABSCharacter : public AModularCharacter, public IAbilitySystemInterface
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

	UFUNCTION(BlueprintCallable, Category = "BS|Character")
	UBSAbilitySystemComponent* GetBSAbilitySystemComponent() const;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

public:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BS|Character", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBSPawnExtensionComponent> PawnExtComponent;

	UPROPERTY()
	TObjectPtr<UBSAbilitySystemComponent> AbilitySystemComponent;
};
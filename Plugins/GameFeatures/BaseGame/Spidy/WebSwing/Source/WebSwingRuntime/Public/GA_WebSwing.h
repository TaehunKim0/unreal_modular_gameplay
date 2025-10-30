// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/BSGameplayAbility.h"
#include "GA_WebSwing.generated.h"

/**
 * 
 */
UCLASS()
class WEBSWINGRUNTIME_API UGA_WebSwing : public UBSGameplayAbility
{
	GENERATED_BODY()

public:
	UGA_WebSwing(const FObjectInitializer& ObjectInitializer);
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

private:
	UFUNCTION()
	void LaunchWebToMouse(FHitResult& OutHitResult);

	UFUNCTION()
	void ExecuteWebSwing(FVector InAttachPoint);
	
	UFUNCTION()
	void ExecuteWebZip(FVector InAttachPoint);

private:
	bool CanUseSwingMode(FVector InAttachPoint);
	
private:
	// Settings
	UPROPERTY(EditDefaultsOnly, Category = "WebSwing")
	float MaxWebDistance = 2000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "WebSwing")
	float SwingAngleThreshold = 45.0f; // 이 각도 이상이면 스윙

	UPROPERTY(EditDefaultsOnly, Category = "WebSwing")
	float SwingForce = 1000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "WebSwing")
	float ZipSpeed = 1500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "WebSwing")
	TEnumAsByte<ECollisionChannel> WebTraceChannel = ECC_Visibility;
};
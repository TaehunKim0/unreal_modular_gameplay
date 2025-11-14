// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/BSGameplayAbility.h"
#include "GA_WebSwing.generated.h"

class AEmitter;
class ACentripetalTestActor;
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
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "WebSwing")
	void SpawnWebSwingVfx(FVector Target, const USkeletalMeshSocket* SourceSocket);

	UFUNCTION(BlueprintImplementableEvent, Category = "WebZip")
	void SpawnWebZipVfx(FVector Target, FVector Source);
	
private:
	UFUNCTION()
	void LaunchWebToMouse(FHitResult& OutHitResult);

	UFUNCTION()
	void ExecuteWebSwing(const FVector& InAttachPoint);
	
	UFUNCTION()
	void ExecuteWebZip(const FVector& InAttachPoint);

private:
	bool CanUseSwingMode(const FVector& InAttachPoint) const;

	UFUNCTION()
	void OnWebSwingFinished();
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "WebZip")
	float MaxWebDistance = 4000.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "WebZip")
	float ZipSpeed = 3000.0f;
	
	UPROPERTY(EditDefaultsOnly, Category = "WebZip")
	float ZipUpwardBoost = 500.f;

private:
	UPROPERTY(EditDefaultsOnly, Category = "WebSwing")
	TEnumAsByte<ECollisionChannel> WebTraceChannel = ECC_Visibility;
};
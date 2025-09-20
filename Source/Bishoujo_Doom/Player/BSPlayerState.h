// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "ModularPlayerState.h"
#include "GameFramework/PlayerState.h"
#include "BSPlayerState.generated.h"

class UBSAbilitySystemComponent;
class UBSCharacterDefinition;
class ABSPlayerController;
class UBSPawnData;
/**
 * 
 */
UCLASS()
class BISHOUJO_DOOM_API ABSPlayerState : public AModularPlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	ABSPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "BS|PlayerState")
	ABSPlayerController* GetBSPlayerController() const;

	
	//~AActor interface
	virtual void PostInitializeComponents() override;
	virtual void GetLifetimeReplicatedProps(class TArray<class FLifetimeProperty,class TSizedDefaultAllocator<32> > &)const override;
	//~End of AActor interface
	
public:
	UFUNCTION(BlueprintCallable, Category = "BS|PlayerState")
	UBSAbilitySystemComponent* GetBSAbilitySystemComponent() const { return AbilitySystemComponent; }
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	const UBSCharacterDefinition* GetCharacterDefData() const { return CharacterDefData; }
	void SetCharacterDefData(const UBSCharacterDefinition* InCharacterDefData);

private:
	 void OnCharacterDefinitionLoaded();

protected:
	UPROPERTY()
	TObjectPtr<const UBSCharacterDefinition> CharacterDefData;

private:
	// The ability system component sub-object used by player characters.
	UPROPERTY(VisibleAnywhere, Category = "BS|PlayerState")
	TObjectPtr<UBSAbilitySystemComponent> AbilitySystemComponent;
};
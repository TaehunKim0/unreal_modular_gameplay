// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BSInputConfig.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "BSInputComponent.generated.h"

struct FGameplayTag;
class UBSInputConfig;
/**
 * 
 */
UCLASS()
class BISHOUJO_DOOM_API UBSInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	UBSInputComponent(const FObjectInitializer& ObjectInitializer);

    // 네이티브 인풋 (어빌리티 제외) + 게임 플레이 태그 바인딩 함수
    template<class UserClass, typename FuncType>
    void BindNativeAction(const UBSInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound = true);
    
    // 어빌리티 인풋 + 게임 플레이 태그 바인딩 함수
    template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
    void BindAbilityActions(const UBSInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);

	void RemoveBinds(TArray<uint32>& BindHandles);
};

template<class UserClass, typename FuncType>
void UBSInputComponent::BindNativeAction(const UBSInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound)
{
	check(InputConfig);
	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag, bLogIfNotFound))
	{
		BindAction(IA, TriggerEvent, Object, Func);
	}
}

template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UBSInputComponent::BindAbilityActions(const UBSInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
{
	check(InputConfig);
    
	for (const FBSInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			// Bind pressed event
			if (PressedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, Action.InputTag).GetHandle());
			}
            
			// Bind released event  
			if (ReleasedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag).GetHandle());
			}
		}
	}
}
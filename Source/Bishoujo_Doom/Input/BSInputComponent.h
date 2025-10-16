// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BSInputSet.h"
#include "BSLogChannels.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "BSInputComponent.generated.h"

struct FGameplayTag;
class UBSInputSet;
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
    void BindNativeAction(const UBSInputSet* InputSet, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound = true);
    
    // 어빌리티 인풋 + 게임 플레이 태그 바인딩 함수
    template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
    void BindAbilityActions(const UBSInputSet* InputSet, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);

	void RemoveBinds(TArray<uint32>& BindHandles);
};

template<class UserClass, typename FuncType>
void UBSInputComponent::BindNativeAction(const UBSInputSet* InputSet, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound)
{
	check(InputSet);
	if (const UInputAction* IA = InputSet->FindNativeInputActionForTag(InputTag, bLogIfNotFound))
	{
		BindAction(IA, TriggerEvent, Object, Func);
		
	}
}

template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UBSInputComponent::BindAbilityActions(const UBSInputSet* InputSet, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
{
	check(InputSet);
    
	for (const FBSInputAction& Action : InputSet->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, Action.InputTag).GetHandle());
			}
            
			if (ReleasedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag).GetHandle());
			}
		}
	}
}
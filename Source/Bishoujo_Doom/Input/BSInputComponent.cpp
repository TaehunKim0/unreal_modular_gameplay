// Fill out your copyright notice in the Description page of Project Settings.


#include "BSInputComponent.h"

UBSInputComponent::UBSInputComponent(const FObjectInitializer& ObjectInitializer)
{
}




void UBSInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
    for (uint32 Handle : BindHandles)
    {
        RemoveBindingByHandle(Handle);
    }
    BindHandles.Reset();
}

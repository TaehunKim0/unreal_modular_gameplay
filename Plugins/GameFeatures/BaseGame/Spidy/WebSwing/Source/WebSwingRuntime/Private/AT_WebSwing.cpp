// Fill out your copyright notice in the Description page of Project Settings.


#include "AT_WebSwing.h"

UAT_WebSwing* UAT_WebSwing::CreateWebSwingTask(UGameplayAbility* InOwningAbility, const FVector& InAttachPoint)
{
	UAT_WebSwing* MyTask = NewAbilityTask<UAT_WebSwing>(InOwningAbility);

	if (MyTask)
	{
		MyTask->TargetAttachPoint = InAttachPoint;
	}
    
	return MyTask;
}

void UAT_WebSwing::Activate()
{
	Super::Activate();

	
	EndTask();
}

void UAT_WebSwing::OnDestroy(bool bInOwnerFinished)
{
	Super::OnDestroy(bInOwnerFinished);
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"
#include "AbilitySystemComponent.h"
#include "MNB_GAS/MNB_GAS.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);

	return MyObj;

}

void UTargetDataUnderMouse::Activate()
{
	const bool bIsLocallyConrolled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if (bIsLocallyConrolled)
	{
		SendMouseCursorData();
	}
	else
	{
		// 先构造一个handle和一个predictionkey
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();

		// ASC系统绑定该函数回调，必要的参数为ABSpechandle和ActivatePredictKey
		AbilitySystemComponent.Get()->
			AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).
			AddUObject(this, &UTargetDataUnderMouse::OnTargetDataReplicatedCallback);

		// 设置之后确保调用委托
		const bool bCalledDelegate = 
			AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictionKey);

		// 还没调用成功
		if (!bCalledDelegate)
		{
			// 通知服务器还没调用委托，等待远程的玩家数据过来
			SetWaitingOnRemotePlayerData();
		}
	}



}

// 输送光标数据
void UTargetDataUnderMouse::SendMouseCursorData()
{
	FScopedPredictionWindow SCopedPrediction(AbilitySystemComponent.Get());


	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Target, false, CursorHit);

	FGameplayAbilityTargetDataHandle DataHandle;

	// 击中目标的Data
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = CursorHit;
	// data添加给Datahandle
	DataHandle.Add(Data);

	FGameplayTag ApplicationTag;
	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey
	);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
{
	// 接收到目标数据时被调用的函数，

	// 告诉客户端已经接收
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey());

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}

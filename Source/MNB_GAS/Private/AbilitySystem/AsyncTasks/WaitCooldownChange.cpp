// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AsyncTasks/WaitCooldownChange.h"
#include "AbilitySystemComponent.h"

UWaitCooldownChange* UWaitCooldownChange::WaitForCooldownChange(UAbilitySystemComponent* ASC, const FGameplayTag& InCooldownTag)
{

	UWaitCooldownChange* WCC = NewObject<UWaitCooldownChange>();	
	WCC->ASC = ASC;
	WCC->CooldownTag = InCooldownTag;

	// 非法
	if (!IsValid(ASC) || !InCooldownTag.IsValid())
	{
		WCC->EndTask();
		return nullptr;
	}

	// CD开始结束（如果广播则代表Cd计时已经结束）
	ASC->RegisterGameplayTagEvent(InCooldownTag, EGameplayTagEventType::NewOrRemoved).
		AddUObject(WCC, &UWaitCooldownChange::CooldownTagChanged);

	// CD被开始应用
	ASC->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(WCC, &UWaitCooldownChange::OnActiveEffectAdded);

	return WCC;
}

void UWaitCooldownChange::EndTask()
{
	if (!IsValid(ASC))
	{
		return;
	}
	// 删除所有CooldonwTag的信息/事件
	ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

	// 标记并删除
	SetReadyToDestroy();
	MarkAsGarbage();
}

// cd标签发生了变化
void UWaitCooldownChange::CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount)
{
	if (NewCount == 0)
	{
		CooldownEnd.Broadcast(0.f);
	}
}

void UWaitCooldownChange::OnActiveEffectAdded(UAbilitySystemComponent* TargetASC, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer AssetTags;
	SpecApplied.GetAllAssetTags(AssetTags);

	FGameplayTagContainer GrantedTags;
	SpecApplied.GetAllGrantedTags(GrantedTags);


	if (AssetTags.HasTagExact(CooldownTag) || GrantedTags.HasTagExact(CooldownTag))
	{
		// 查询所有拥有的标签，构造一个Container实例
		FGameplayEffectQuery GameplayEffectQuery =
			FGameplayEffectQuery::MakeQuery_MatchAllOwningTags(CooldownTag.GetSingleTagContainer());
		// 获取GE的剩余时间
		TArray<float> TimesRemaining = ASC->GetActiveEffectsTimeRemaining(GameplayEffectQuery);
		if(TimesRemaining.Num() > 0)
		{
			// 记录当前所有处于cd中的cd最大值
			float TimeRemaining = TimesRemaining[0];
			for (int32 i = 0; i < TimesRemaining.Num(); i++)
			{
				if (TimesRemaining[i] > TimeRemaining)
				{
					TimeRemaining = TimesRemaining[i];

				}
			}
			CooldownStart.Broadcast(TimeRemaining);
		}
		//ASC->GetActiveEffectsTimeRemaining()
	}
}

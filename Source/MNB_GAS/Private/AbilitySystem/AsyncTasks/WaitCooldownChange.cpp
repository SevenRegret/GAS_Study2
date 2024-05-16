// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AsyncTasks/WaitCooldownChange.h"
#include "AbilitySystemComponent.h"

UWaitCooldownChange* UWaitCooldownChange::WaitForCooldownChange(UAbilitySystemComponent* ASC, const FGameplayTag& InCooldownTag)
{

	UWaitCooldownChange* WCC = NewObject<UWaitCooldownChange>();	
	WCC->ASC = ASC;
	WCC->CooldownTag = InCooldownTag;

	// �Ƿ�
	if (!IsValid(ASC) || !InCooldownTag.IsValid())
	{
		WCC->EndTask();
		return nullptr;
	}

	// CD��ʼ����������㲥�����Cd��ʱ�Ѿ�������
	ASC->RegisterGameplayTagEvent(InCooldownTag, EGameplayTagEventType::NewOrRemoved).
		AddUObject(WCC, &UWaitCooldownChange::CooldownTagChanged);

	// CD����ʼӦ��
	ASC->OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(WCC, &UWaitCooldownChange::OnActiveEffectAdded);

	return WCC;
}

void UWaitCooldownChange::EndTask()
{
	if (!IsValid(ASC))
	{
		return;
	}
	// ɾ������CooldonwTag����Ϣ/�¼�
	ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);

	// ��ǲ�ɾ��
	SetReadyToDestroy();
	MarkAsGarbage();
}

// cd��ǩ�����˱仯
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
		// ��ѯ����ӵ�еı�ǩ������һ��Containerʵ��
		FGameplayEffectQuery GameplayEffectQuery =
			FGameplayEffectQuery::MakeQuery_MatchAllOwningTags(CooldownTag.GetSingleTagContainer());
		// ��ȡGE��ʣ��ʱ��
		TArray<float> TimesRemaining = ASC->GetActiveEffectsTimeRemaining(GameplayEffectQuery);
		if(TimesRemaining.Num() > 0)
		{
			// ��¼��ǰ���д���cd�е�cd���ֵ
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

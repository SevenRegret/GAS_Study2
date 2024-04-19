// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());

	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());

	/*AbilitySystemComponent->
		GetGameplayAttributeValueChangedDelegate(AuraAttributeSet->GetHealthAttribute().AddUObject();*/
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributeSet->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributeSet->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributeSet->GetManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnManaChanged.Broadcast(Data.NewValue);
			}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AuraAttributeSet->GetMaxManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxManaChanged.Broadcast(Data.NewValue);
			}
	);

	// 为GetHealthAttribute属性值 addUObject
	//AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
	//		AuraAttributeSet->GetHealthAttribute()).AddUObject(
	//			this, &UOverlayWidgetController::HealthChanged);

	//AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
	//	AuraAttributeSet->GetMaxHealthAttribute()).AddUObject(
	//		this, &UOverlayWidgetController::MaxHealthChanged);

	//AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
	//	AuraAttributeSet->GetManaAttribute()).AddUObject(
	//		this, &UOverlayWidgetController::ManaChanged);

	//AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
	//	AuraAttributeSet->GetMaxManaAttribute()).AddUObject(
	//		this, &UOverlayWidgetController::MaxManaChanged);


	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
		[this](const FGameplayTagContainer& AssetTags)
		{

			for (const FGameplayTag& Tag : AssetTags)
			{
				// 匹配("A")则返回true， ("A.1")返回false
				// 这里只期望找到属于Message的Tag，其他任何Message之外的不关注
				FGameplayTag MassegeTag = FGameplayTag::RequestGameplayTag(FName("Message"));

				if (Tag.MatchesTag(MassegeTag))
				{
					const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);

					// 利用委托进行广播
					MessageWidgetRowDelegate.Broadcast(*Row);
				}
			}
		}
	);
}
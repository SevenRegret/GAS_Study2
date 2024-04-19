// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	// 委托
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);


	// GameplayTags获取单例
	//const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	////GameplayTags.Attributes_Secondary_Armor.ToString();
	//GEngine->AddOnScreenDebugMessage(
	//	-1,
	//	10.f,
	//	FColor::Orange,
	//	FString::Printf(TEXT("Tag: %s"), *GameplayTags.Attributes_Secondary_Armor.ToString())
	//);
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);

		// Spec中拿到Ab信息
		if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);
			// 赋予AB
			GiveAbility(AbilitySpec);
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	// 从所有已经激活的AB中遍历
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// 严格匹配InputTag
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			// 这里仅仅通知Ab，该能力处于一个pressed状态
			AbilitySpecInputPressed(AbilitySpec);

			if (!AbilitySpec.IsActive())
			{
				// 尝试激活AB
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
	// 从所有已经激活的AB中遍历
	/*for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
		{
			// 严格匹配InputTag
			if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
			{
				// 这里仅仅通知Ab，该能力处于一个pressed状态
				AbilitySpecInputPressed(AbilitySpec);

				if (!AbilitySpec.IsActive())
				{
					// 尝试激活AB
					TryActivateAbility(AbilitySpec.Handle);
				}
			}
		}
		*/

}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	// 从所有已经激活的AB中遍历
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// 严格匹配InputTag
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			// 这里仅仅通知Ab，该能力处于一个pressed状态
			AbilitySpecInputReleased(AbilitySpec);
		}
	}

}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(
	UAbilitySystemComponent* AbilitySystemComponent, 
	const FGameplayEffectSpec& EffectSpec, 
	FActiveGameplayEffectHandle ActiveEffectHandle)
{
	//GEngine->AddOnScreenDebugMessage(1, 8.f, FColor::Blue, FString("Effect Applied"));

	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	EffectAssetTags.Broadcast(TagContainer);

}

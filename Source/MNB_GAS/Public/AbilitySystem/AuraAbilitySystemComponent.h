// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

class ULoadScreenSaveGame;
struct FGameplayTag;

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer&/*AssetTags*/);

// 能力被赋予时广播
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven);

// 单个能力绑定一个委托
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&);

// 技能状态时变化委托
DECLARE_MULTICAST_DELEGATE_ThreeParams(
	FAbilityStatusChanged, 
	const FGameplayTag& /*AbilityTag*/, 
	const FGameplayTag& /*StatusTag*/,
	int32/*AbilityLevel*/);


// 技能状态时变化委托
DECLARE_MULTICAST_DELEGATE_FourParams(
	FAbilityEquipped,
	const FGameplayTag& /*AbilityTag*/,
	const FGameplayTag& /*StatusTag*/,
	const FGameplayTag& /*Slotg*/, 
	const FGameplayTag& /*PrevSlot*/);

DECLARE_MULTICAST_DELEGATE_OneParam(FDeactivatePassiveAbility, const FGameplayTag&);

DECLARE_MULTICAST_DELEGATE_TwoParams(FActivatePassiveEffect, const FGameplayTag&, bool);


/**
 * 
 */
UCLASS()
class MNB_GAS_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	void AbilityActorInfoSet();


	FEffectAssetTags EffectAssetTags;
	FAbilitiesGiven AbilitiesGivenDelegate;
	FAbilityStatusChanged AbilityStatusChanged;
	FAbilityEquipped AbilityEquipped;
	FDeactivatePassiveAbility DeactivatePassiveAbility;
	FActivatePassiveEffect ActivatePassiveEffect;


	void AddCharacterAbilitiesFromSaveData(ULoadScreenSaveGame* SaveData);
	// 添加技能组(这里是初始化时调用
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);

	void AddCharacterPassvieAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassvieAbilities);
	// 用于确保技能都已经添加完毕，此时才可以绑定技能被赋予的委托
	bool bStartupAbilitiesGiven = false;

	// 检测按键何时被释放以及按住
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	// TODO
	void ForEachAbility(const FForEachAbility& Delegate);

	// 从Spec中获取GTag
	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec);

	FGameplayTag GetStatusFromAbilityTag(const FGameplayTag& AbilityTag);
	FGameplayTag GetSlotFromAbilityTag(const FGameplayTag& AbilityTag);
	bool SlotIsEmpty(const FGameplayTag& Slot);
	static bool AbilityHasSlot(const FGameplayAbilitySpec& Spec, const FGameplayTag& Slot);
	static bool AbilityHasAnySlot(const FGameplayAbilitySpec& Spec);
	FGameplayAbilitySpec* GetSpecWithSlot(const FGameplayTag& Slot);
	bool IsPassiveAbility(const FGameplayAbilitySpec& Spec) const;

	void AssignSlotToAbility(FGameplayAbilitySpec& Spec, const FGameplayTag& Slot);

	// 给角色激活被动技能的GE特效
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastActivatePassiveEffect(const FGameplayTag& AbilityTag, bool bActivate);

	
	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag);

	void UpgradeAttribute(const FGameplayTag& AttributeTag);


	// 网络复制，通过tag更新
	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);

	// 更新技能组根据而等级
	void UpdateAbilityStatues(int32 Level);

	UFUNCTION(Server, Reliable)
	// 佩戴技能触发
	void ServerEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& Slot);

	UFUNCTION(Server, Reliable)
	void ServerSpendSpellPoint(const FGameplayTag& AbilityTag);

	UFUNCTION(Client, Reliable)
	void ClientEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot);

	// 根据Tag查找描述
	bool GetDescriptionByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription, FString& OutNextLevelDescription);

	// 清除对应槽位的技能
	static void ClearSlot(FGameplayAbilitySpec* Spec);
	// 清除技能（根据槽位
	void ClearAbilitiesOfSlot(const FGameplayTag& Slot);

	// 判断技能是否在槽位中
	static bool AbilityHasSlot(FGameplayAbilitySpec* Spec, const FGameplayTag& Slot);
protected:

	// 复制激活能力
	virtual void OnRep_ActivateAbilities() override;

	// 客户端也可以调用effectApply
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffetSpec,
		FActiveGameplayEffectHandle ActiveEffectHandle);

	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityStatus(
		const FGameplayTag& AbilityTag, 
		const FGameplayTag& StatusTag,
		int32 AbilityLevel);
};

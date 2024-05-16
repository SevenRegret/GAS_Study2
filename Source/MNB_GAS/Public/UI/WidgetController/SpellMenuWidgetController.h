// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "GameplayTagContainer.h"
#include "AuraGameplayTags.h"
#include "SpellMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
	FSpellGlobeSelectedSignature, 
	bool, bSpendPointsButtonEnabled, 
	bool, bEquipButtonEnabled,
	FString, DescriptionString,
	FString, NextLevelDescriptionString);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FWaitForEquipSelectionSignature,
	const FGameplayTag&, AbilityTypeTag);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FSpellGlobeReassignedSignature,
	const FGameplayTag&, AbilityTag);


struct FSelectedAbility
{
	FGameplayTag Ability = FGameplayTag();
	FGameplayTag Status = FGameplayTag();
};
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class MNB_GAS_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
	
public:
	virtual void BroadcastInitialValues() override;

	virtual void BindCallbacksToDependencies() override;

	// 技能点变动委托
	UPROPERTY(BlueprintAssignable)
	FOnPlayerStateChangedSignature SpellPointsChanged;

	// 被选中时的委托
	UPROPERTY(BlueprintAssignable)
	FSpellGlobeSelectedSignature SpellGlobeSelectedDelegate;

	UPROPERTY(BlueprintAssignable)
	FWaitForEquipSelectionSignature  WaitForEquipDelegate;

	UPROPERTY(BlueprintAssignable)
	FWaitForEquipSelectionSignature  StopWaitingForEquipDelegate;

	UPROPERTY(BlueprintAssignable)
	FSpellGlobeReassignedSignature SpellGlobeRessignedDelegate;

	// 选中加点界面
	UFUNCTION(BlueprintCallable)
	void SpellGlobeSelected(const FGameplayTag& AbilityTag);


	UFUNCTION(BlueprintCallable)
	void SpendPointButtonPressed();


	UFUNCTION(BlueprintCallable)
	void GlobeDeselect();

	// 装备技能
	UFUNCTION(BlueprintCallable)
	void EquipButtonPressed();

	UFUNCTION(BlueprintCallable)
	void SpellRowGlobePressed(const FGameplayTag& SlotTag, const FGameplayTag& InAbilityTypeTag);


	// 技能佩戴
	void OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot);
private:
	void ShouldEnableButtons(
		const FGameplayTag& AbilityStatusTag, 
		int32 SpellPoints, 
		bool& bShouldEnableSpellPointsButton, 
		bool& bShouldEnableEquipButton);

	// 选中Tag组合
	FSelectedAbility SelectedAbility = 
	{ FAuraGameplayTags::Get().Abilities_None, FAuraGameplayTags::Get().Abilities_Status_Locked };

	int CurrentSpellPoints = 0;

	bool bWaitingForEquipSelection = false;

	// 选择了第几号槽位
	FGameplayTag SelectedSlot;
};

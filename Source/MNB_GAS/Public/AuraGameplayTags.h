// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"


/**
 *	用于自定义标签, 包含native Gameplay Tags的单例
 */
struct FAuraGameplayTags
{
public:
	static const FAuraGameplayTags& Get(){ return GameplayTags; }

	// 初始化Tags
	static void InitializeNativeGameplayTags();

	// 一级属性
	FGameplayTag Attributes_Primary_Strength;

	FGameplayTag Attributes_Primary_Intelligence;

	FGameplayTag Attributes_Primary_Resilience;

	FGameplayTag Attributes_Primary_Vigor;



	// 二级属性Tag -- 护甲
	FGameplayTag Attributes_Secondary_Armor;

	FGameplayTag Attributes_Secondary_ArmorPenetration;

	FGameplayTag Attributes_Secondary_BlockChance;

	FGameplayTag Attributes_Secondary_CriticalHitChance;

	FGameplayTag Attributes_Secondary_CriticalHitDamage;

	FGameplayTag Attributes_Secondary_CriticalHitResistance;

	FGameplayTag Attributes_Secondary_HealthRegeneration;

	FGameplayTag Attributes_Secondary_ManaRegeneration;

	FGameplayTag Attributes_Secondary_MaxHealth;

	FGameplayTag Attributes_Secondary_MaxMana;

	// 元素抗性
	FGameplayTag Attributes_Resistance_Fire;
	FGameplayTag Attributes_Resistance_Lighting;
	FGameplayTag Attributes_Resistance_Arcane;
	FGameplayTag Attributes_Resistance_Physics;


	// 鼠标左右键输入Tag
	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;
	FGameplayTag InputTag_1;
	FGameplayTag InputTag_2;
	FGameplayTag InputTag_3;
	FGameplayTag InputTag_4;
	FGameplayTag InputTag_Passive_1;
	FGameplayTag InputTag_Passive_2;

	// 伤害
	FGameplayTag Damage;

	FGameplayTag Damage_Fire;
	FGameplayTag Damage_Lighting;
	FGameplayTag Damage_Arcane;
	FGameplayTag Damage_Physics;


	// 标识debuff
	FGameplayTag Debuff_Burn;
	FGameplayTag Debuff_Stun;
	FGameplayTag Debuff_Acrane;
	FGameplayTag Debuff_Physical;

	FGameplayTag Debuff_Set_Chance;
	FGameplayTag Debuff_Set_Damage;
	FGameplayTag Debuff_Set_Duration;
	FGameplayTag Debuff_Set_Frequency;

	FGameplayTag Attributes_Meta_IncomingXP;
	
	// Ability， 标识技能
	FGameplayTag Abilities_Attack;
	FGameplayTag Abilities_Summon;

	FGameplayTag Abilities_HitReact;

	FGameplayTag Abilities_None;

	FGameplayTag Abilities_Status_Locked;
	FGameplayTag Abilities_Status_Eligible;
	FGameplayTag Abilities_Status_Unlocked;
	FGameplayTag Abilities_Status_Equipped;

	FGameplayTag Abilities_Type_Offensive;
	FGameplayTag Abilities_Type_Passive;
	FGameplayTag Abilities_Type_None;

	// 技能实例标签
	FGameplayTag Abilities_Fire_Firebolt;
	FGameplayTag Abilities_Fire_FireBlast;
	FGameplayTag Abilities_Lighting_Electrocute;
	FGameplayTag Abilities_Acrane_AcraneShards;



	FGameplayTag Abilities_Passive_HaloOfProtection;
	FGameplayTag Abilities_Passive_LifeSpan;
	FGameplayTag Abilities_Passive_ManaSiphon;

	// 技能cd
	FGameplayTag Cooldown_Fire_Firebolt;


	// 蒙太奇攻击标签
	FGameplayTag CombatSocket_Weapon;
	FGameplayTag CombatSocket_RightHand;
	FGameplayTag CombatSocket_LeftHand;
	FGameplayTag CombatSocket_Tail;

	FGameplayTag Montage_Attack_1;
	FGameplayTag Montage_Attack_2;
	FGameplayTag Montage_Attack_3;
	FGameplayTag Montage_Attack_4;

	// 伤害类型和抵抗类型
	TMap<FGameplayTag, FGameplayTag> DamageTypesToResistances;
	// debuff类型和伤害类型
	TMap<FGameplayTag, FGameplayTag> DamageTypesToDebuff;

	// 受击Tag
	FGameplayTag Effects_HitReact;

	FGameplayTag Player_Block_InputPressed;
	FGameplayTag Player_Block_InputHeld;
	FGameplayTag Player_Block_InputReleased;
	FGameplayTag Player_Block_CursorTrace;

	
	FGameplayTag GameplayCue_FireBlast;
protected:


private:
	static FAuraGameplayTags GameplayTags;
};


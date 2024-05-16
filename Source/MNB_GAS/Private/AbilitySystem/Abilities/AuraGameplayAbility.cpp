// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/AuraAttributeSet.h"

FString UAuraGameplayAbility::GetDescription(int32 Level)
{
    return FString::Printf(TEXT("<Default>暂无描述信息</><Level>%d</>"), Level);
}

FString UAuraGameplayAbility::GetNextLevelDescription(int32 Level)
{
    return FString::Printf(TEXT("<Default>下一级：</><Level>%d</>"), Level);
}

FString UAuraGameplayAbility::GetLockedDescription(int32 Level)
{
    return FString::Printf(TEXT("<Default>解锁等级：</><Level>%d</>"), Level);
}

float UAuraGameplayAbility::GetManaCost(float InLevel) const 
{
    float ManaCost = 0.f;
    // 技能需要拿到对应costGE
    if (const UGameplayEffect* CostEffect = GetCostGameplayEffect())
    {
        // Ge里面找Modifire类型（判断是否是你需要的这个cost
        for (FGameplayModifierInfo Mode : CostEffect->Modifiers)
        {
            if (Mode.Attribute == UAuraAttributeSet::GetManaAttribute())
            {
                Mode.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, ManaCost);
                break;
            }
        }
    }
    return ManaCost;
}

float UAuraGameplayAbility::GetCooldown(float InLevel) const 
{
    float Cooldown = 0;
    if (const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
    {
        CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel, Cooldown);
    }
    return Cooldown;
}

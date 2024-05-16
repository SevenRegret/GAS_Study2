// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Electrocute.h"

FString UElectrocute::GetDescription(int32 Level)
{
	const float ScaledDamage = DamageCurve.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		return FString::Printf(TEXT(
			"<Title>万雷天牢引</>\n\n"
			//des
			"<Default>介绍:释放闪电造成</><Damage>%.1f</><Default>点雷电伤害.</>\n"
			//level
			"<Small>等级:</><Level>%d</>\n"
			//cost
			"<Small>消耗:</><ManaCost>%.1f</><Small>法力值</>\n"
			"<Small>冷却:</><Cooldown>%.1f</><Small>S</>"
		), ScaledDamage, Level, ManaCost, Cooldown);

	}
	else
	{
		return FString::Printf(TEXT(
			"<Title>万雷天牢引:</>\n\n"
			//des
			"<Default>介绍:释放闪电对%d个敌人造成</><Damage>%.1f</><Default>点雷电伤害.</>\n"
			//level
			"<Small>Level:</><Level>%d</>\n"
			//cost
			"<Small>法力:</><ManaCost>%.1f</><Small>法力值</>\n"
			"<Small>冷却:</><Cooldown>%.1f</><Small>S</>"
		), FMath::Min(Level, MaxNumShockTargets), ScaledDamage, Level, ManaCost, Cooldown);

	}

	return FString();
}

FString UElectrocute::GetNextLevelDescription(int32 Level)
{
	const float ScaledDamage = DamageCurve.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);

	return FString::Printf(TEXT(
		"<Title>万雷天牢引:</>\n\n"
		//des
		"<Default>介绍:释放闪电对%d个敌人造成</><Damage>%.1f</><Default>点雷电伤害.</>\n"
		//level
		"<Small>Level:</><Level>%d</>\n"
		//cost
		"<Small>法力:</><ManaCost>%.1f</><Small>法力值</>\n"
		"<Small>冷却:</><Cooldown>%.1f</><Small>S</>"
	), FMath::Min(Level, MaxNumShockTargets), ScaledDamage, Level, ManaCost, Cooldown);
}

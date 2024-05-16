// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/ArcaneShards.h"

FString UArcaneShards::GetDescription(int32 Level)
{
	const float ScaledDamage = DamageCurve.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		return FString::Printf(TEXT(
			"<Title>奥术：水晶地刺</>\n\n"
			//des
			"<Default>介绍:从目标地点召唤一团水晶向上突刺对命中的敌人造成</><Damage>%.1f</><Default>点物理伤害. 并将其击飞一段距离</>\n"
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
			"<Default>介绍:从目标地点召唤%d团水晶向上突刺对命中的敌人造成</><Damage>%.1f</><Default>点物理伤害. 并将其击飞一段距离</>\n"
			//level
			"<Small>Level:</><Level>%d</>\n"
			//cost
			"<Small>法力:</><ManaCost>%.1f</><Small>法力值</>\n"
			"<Small>冷却:</><Cooldown>%.1f</><Small>S</>"
		), FMath::Min(Level, MaxNumShards), ScaledDamage, Level, ManaCost, Cooldown);

	}

	return FString();
}

FString UArcaneShards::GetNextLevelDescription(int32 Level)
{
	const float ScaledDamage = DamageCurve.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);

	return FString::Printf(TEXT(
		"<Title>万雷天牢引:</>\n\n"
		//des
		"<Default>介绍:从目标地点召唤%d团水晶向上突刺对命中的敌人造成</><Damage>%.1f</><Default>点物理伤害. 并将其击飞一段距离</>\n"
		//level
		"<Small>Level:</><Level>%d</>\n"
		//cost
		"<Small>法力:</><ManaCost>%.1f</><Small>法力值</>\n"
		"<Small>冷却:</><Cooldown>%.1f</><Small>S</>"
	), FMath::Min(Level, MaxNumShards), ScaledDamage, Level, ManaCost, Cooldown);
}

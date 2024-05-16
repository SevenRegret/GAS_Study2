// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraFireBlast.h"

#include "Actor/AuraFireBall.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"

FString UAuraFireBlast::GetDescription(int32 Level)
{
	const float ScaledDamage = DamageCurve.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);

	return FString::Printf(TEXT(
		"<Title>豪火灭却</>\n\n"
		//des
		"<Default>介绍:释放一团火球对目标方向第一个命中的敌人造成</><Damage>%.1f</><Default>点火焰伤害.</>\n"
		//level
		"<Small>等级:</><Level>%d</>\n"
		//cost
		"<Small>消耗:</><ManaCost>%.1f</><Small>法力值</>\n"
		"<Small>冷却:</><Cooldown>%.1f</><Small>S</>"
	), ScaledDamage, Level, ManaCost, Cooldown);
}

FString UAuraFireBlast::GetNextLevelDescription(int32 Level)
{
    return GetDescription(Level);
}

TArray<AAuraFireBall*> UAuraFireBlast::SpawnFireBalls()
{
	TArray<AAuraFireBall*> FireBalls;
	// 角色向前向量和位置
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	
	// 旋转量
	TArray<FRotator> Rotators = UAuraAbilitySystemLibrary::EvenlySpaceRotators(Forward, FVector::UpVector, 360.f, NumFireBalls);
	for (const FRotator& Rotator : Rotators)
	{
		// transform
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(Location);
		SpawnTransform.SetRotation(Rotator.Quaternion());

		AAuraFireBall* FireBall = GetWorld()->SpawnActorDeferred<AAuraFireBall>(
			FireBallClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			CurrentActorInfo->PlayerController->GetPawn(),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		// 最后设置DamageEffect
		FireBall->DamageEffectParams = MakeDamageEffectParamsClassDefaults();
		FireBall->ReturnToActor = GetAvatarActorFromActorInfo();
		FireBall->SetOwner(GetAvatarActorFromActorInfo());

		FireBall->ExplosionDamageParams = MakeDamageEffectParamsClassDefaults();
		FireBall->SetOwner(GetAvatarActorFromActorInfo());

		FireBalls.Add(FireBall);
		FireBall->FinishSpawning(SpawnTransform);
	}
	return TArray<AAuraFireBall*>();
}

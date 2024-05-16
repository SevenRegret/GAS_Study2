// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

FString UAuraFireBolt::GetDescription(int32 Level)
{
	const float ScaledDamage = DamageCurve.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	if (Level == 1)
	{
		return FString::Printf(TEXT(
			"<Title>火球术</>\n\n"
			//des
			"<Default>介绍:释放一团火球对目标方向第一个命中的敌人造成</><Damage>%.1f</><Default>点火焰伤害.</>\n"
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
			"<Title>火球术:</>\n\n"
			//des
			"<Default>释放%d团火球对目标方向第一个命中的敌人造成</><Damage>%.1f</><Default>点火焰伤害.</>\n"
			//level
			"<Small>Level:</><Level>%d</>\n"
			//cost
			"<Small>法力:</><ManaCost>%.1f</><Small>法力值</>\n"
			"<Small>冷却:</><Cooldown>%.1f</><Small>S</>"
		), FMath::Min(Level, NumProjectiles), ScaledDamage, Level, ManaCost, Cooldown);

	}

	return FString();
}

FString UAuraFireBolt::GetNextLevelDescription(int32 Level)
{
	const float ScaledDamage = DamageCurve.GetValueAtLevel(Level);
	const float ManaCost = FMath::Abs(GetManaCost(Level));
	const float Cooldown = GetCooldown(Level);
	return FString::Printf(TEXT(
		"<Title>下一等级:</>\n\n"
		//des
		"<Default>释放%d团火球对目标方向第一个命中的敌人造成</><Damage>%.1f</><Default>点火焰伤害.</>\n"
		//level
		"<Small>Level:</><Level>%d</>\n"
		//cost
		"<Small>法力:</><ManaCost>%.1f</><Small>法力值</>\n"
		"<Small>冷却:</><Cooldown>%.1f</><Small>S</>"
	), FMath::Min(Level, NumProjectiles), ScaledDamage, Level, ManaCost, Cooldown);
}

void UAuraFireBolt::SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride, AActor* HomingTarget)
{
	// 通过激活信息判断是否为服务器
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();

	if (!bIsServer) return;

	// 1. 获取应该在哪里生成，CombatSocket，武器还是手还是其他
	const FVector SocketLocation =
		ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);

	// 2. 弹体目标方向和武器socket位置做差值得到旋转
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();



	// 3. 如果需要重置高度， 弹体朝向和数量等设置
	if (bOverridePitch)
	{
		Rotation.Pitch = PitchOverride;
	}

	const FVector Forward = Rotation.Vector();
	const int32 EffectiveNumProjectiles = FMath::Min(NumProjectiles, GetAbilityLevel());

	// 根据弹体数量求各自旋转
	TArray<FRotator> Rotations =
		UAuraAbilitySystemLibrary::EvenlySpaceRotators(Forward, FVector::UpVector, ProjectileSpread, EffectiveNumProjectiles);


	for (FRotator& Rotator : Rotations)
	{

		// 在aura角色的武器处生成弹体，则需要其武器的socket接口
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		// TODO 设置弹体的旋转矢量等
		SpawnTransform.SetRotation(Rotator.Quaternion());

		// 设置每个Projectile的参数（类实例，Transform，等）
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);

		Projectile->DamageEffectParams = MakeDamageEffectParamsClassDefaults();


		if (HomingTarget && HomingTarget->Implements<UCombatInterface>())
		{
			Projectile->ProjectileMovement->HomingTargetComponent = HomingTarget->GetRootComponent();
		}
		else
		{
			Projectile->HomingTargetSceneComponent = NewObject<USceneComponent>(USceneComponent::StaticClass());
			Projectile->HomingTargetSceneComponent->SetWorldLocation(ProjectileTargetLocation);
			Projectile->ProjectileMovement->HomingTargetComponent = Projectile->HomingTargetSceneComponent;
		}

		Projectile->ProjectileMovement->HomingAccelerationMagnitude = FMath::FRandRange(HomingAcceleration, HomingAccelerationMax);
		Projectile->ProjectileMovement->bIsHomingProjectile = bLaunchHoimingProjectiles;


		Projectile->FinishSpawning(SpawnTransform);
	}

}

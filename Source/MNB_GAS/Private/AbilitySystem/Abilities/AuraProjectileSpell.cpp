// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "Actor/AuraProjectile.h"
#include "Character/AuraCharacterBase.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include <AuraGameplayTags.h>



void UAuraProjectileSpell::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);


	

}

// 简单的生成弹体逻辑
void UAuraProjectileSpell::SpawnProjectile(
	const FVector& ProjectileTargetLocation, 
	const FGameplayTag& SocketTag, 
	bool bOverridePitch,
	float PitchOverride)
{
	// 通过激活信息判断是否为服务器
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();

	if (!bIsServer) return;

	const FVector SocketLocation = 
		ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);

	// 弹体目标方向和武器socket位置做差值得到旋转
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();


	// 在aura角色的武器处生成弹体，则需要其武器的socket接口
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	// TODO 设置弹体的旋转矢量等
	SpawnTransform.SetRotation(Rotation.Quaternion());

	// 如果需要重置高度
	if (bOverridePitch)
	{
		Rotation.Pitch = PitchOverride;
	}


	AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	// 3. 指定弹体EffectParams
	Projectile->DamageEffectParams = MakeDamageEffectParamsClassDefaults();

	// 最后必须finish生成 
	Projectile->FinishSpawning(SpawnTransform);

}

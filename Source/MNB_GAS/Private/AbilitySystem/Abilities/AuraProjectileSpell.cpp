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

	// TODO 给弹体一个GE效果 设置弹体伤害
	// 伤害GE1.从avatarActor拿到ASC
	const UAbilitySystemComponent* SourceASC =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());

	// EffectContext设置
	FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
	EffectContextHandle.SetAbility(this);
	EffectContextHandle.AddSourceObject(Projectile);
	TArray<TWeakObjectPtr<AActor>> Actors;
	Actors.Add(Projectile);
	EffectContextHandle.AddActors(Actors);
	FHitResult HitResult;
	HitResult.Location = ProjectileTargetLocation;
	EffectContextHandle.AddHitResult(HitResult);


	// 2.
	const FGameplayEffectSpecHandle SpecHandle =
		SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), SourceASC->MakeEffectContext());

	// 生成弹体同时赋予其DamageTag(配合damageGe中的SetByCaller选项
	const FAuraGameplayTags  GameplayTags = FAuraGameplayTags::Get();


	// 遍历伤害类型，获取其伤害值
	for (auto& Pair : DamageTypes)
	{
		const float ScaleDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaleDamage);
	}

	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("FireBolt Damage: %f"), ScaleDamage));

	// 按配置的CurveTable配置应该造成的伤害
	//UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Damage, ScaleDamage);


	// 3. 指定弹体Handle
	Projectile->DamageEffectSpecHandle = SpecHandle;
	// 最后必须finish生成 
	Projectile->FinishSpawning(SpawnTransform);

}

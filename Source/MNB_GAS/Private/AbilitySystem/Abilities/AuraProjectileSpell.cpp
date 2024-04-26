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

// �򵥵����ɵ����߼�
void UAuraProjectileSpell::SpawnProjectile(
	const FVector& ProjectileTargetLocation, 
	const FGameplayTag& SocketTag, 
	bool bOverridePitch,
	float PitchOverride)
{
	// ͨ��������Ϣ�ж��Ƿ�Ϊ������
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();

	if (!bIsServer) return;

	const FVector SocketLocation = 
		ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);

	// ����Ŀ�귽�������socketλ������ֵ�õ���ת
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();


	// ��aura��ɫ�����������ɵ��壬����Ҫ��������socket�ӿ�
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(SocketLocation);
	// TODO ���õ������תʸ����
	SpawnTransform.SetRotation(Rotation.Quaternion());

	// �����Ҫ���ø߶�
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

	// TODO ������һ��GEЧ�� ���õ����˺�
	// �˺�GE1.��avatarActor�õ�ASC
	const UAbilitySystemComponent* SourceASC =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());

	// EffectContext����
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

	// ���ɵ���ͬʱ������DamageTag(���damageGe�е�SetByCallerѡ��
	const FAuraGameplayTags  GameplayTags = FAuraGameplayTags::Get();


	// �����˺����ͣ���ȡ���˺�ֵ
	for (auto& Pair : DamageTypes)
	{
		const float ScaleDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Pair.Key, ScaleDamage);
	}

	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString::Printf(TEXT("FireBolt Damage: %f"), ScaleDamage));

	// �����õ�CurveTable����Ӧ����ɵ��˺�
	//UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Damage, ScaleDamage);


	// 3. ָ������Handle
	Projectile->DamageEffectSpecHandle = SpecHandle;
	// ������finish���� 
	Projectile->FinishSpawning(SpawnTransform);

}

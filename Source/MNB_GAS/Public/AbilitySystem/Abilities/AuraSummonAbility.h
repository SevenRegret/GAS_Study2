// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraSummonAbility.generated.h"

/**
 *  召唤GA
 */
UCLASS()
class MNB_GAS_API UAuraSummonAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
	
	// 生成地点Array
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetSpawnLocations();

	// 获取随机的召唤类型
	UFUNCTION(BlueprintPure, Category = "Summoning")
	TSubclassOf<APawn> GetRandomMinionClass();

	// 数量
	UPROPERTY(EditDefaultsOnly, Category = "Summoning")
	int32 NumMinions = 0;

	// 需要生成的小崽子类型
	UPROPERTY(EditDefaultsOnly, Category = "Summoning")
	TArray<TSubclassOf<APawn>> MinionClasses;

	UPROPERTY(EditDefaultsOnly, Category = "Summoning")
	float MinSpawnDistance = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "Summoning")
	float MaxSpawnDistance = 250.f;

	// 最大召唤扩散角度
	UPROPERTY(EditDefaultsOnly, Category = "Summoning")
	float SpawnSpread = 90.f;


};

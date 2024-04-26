// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraSummonAbility.generated.h"

/**
 *  �ٻ�GA
 */
UCLASS()
class MNB_GAS_API UAuraSummonAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
	
	// ���ɵص�Array
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetSpawnLocations();

	// ��ȡ������ٻ�����
	UFUNCTION(BlueprintPure, Category = "Summoning")
	TSubclassOf<APawn> GetRandomMinionClass();

	// ����
	UPROPERTY(EditDefaultsOnly, Category = "Summoning")
	int32 NumMinions = 0;

	// ��Ҫ���ɵ�С��������
	UPROPERTY(EditDefaultsOnly, Category = "Summoning")
	TArray<TSubclassOf<APawn>> MinionClasses;

	UPROPERTY(EditDefaultsOnly, Category = "Summoning")
	float MinSpawnDistance = 50.f;

	UPROPERTY(EditDefaultsOnly, Category = "Summoning")
	float MaxSpawnDistance = 250.f;

	// ����ٻ���ɢ�Ƕ�
	UPROPERTY(EditDefaultsOnly, Category = "Summoning")
	float SpawnSpread = 90.f;


};

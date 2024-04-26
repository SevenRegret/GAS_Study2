// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Data/CharacterClassInfo.h"
#include "AuraAbilitySystemLibrary.generated.h"

class UOverlayWidgetController;
class UAttributeMenuWidgetController;

class UAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class MNB_GAS_API UAuraAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	

public:
	// 静态函数无法调用world中的任何Object，因为其类可能不存在，因此静态函数需要一个上下文
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController")
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController")
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);


	// 这是为Enemy准备的, Character直接在玩家类中初始化赋予了
	// 初始化默认属性集，
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|CharacterClassDefaults")
	static void InitializedDefaultAttributes(
		const UObject* WorldContextObject, 
		ECharacterClass CharacterClass, 
		float Level, 
		UAbilitySystemComponent* ASC);

	// 给与初始化的能力到Enemy
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|CharacterClassDefaults")
	static void GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|CharacterClassDefaults")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInisBlockedHit);

	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInisCriticalHit);

	// 获取一定半径范围内的或者的player
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static void GetLivePlayersWithRadius(
		const UObject* WorldContextObject, 
		TArray<AActor*>& OutOverlappingActors, 
		const TArray<AActor*>& ActorsToIgnore,
		float Radius,
		const FVector& SphereOrigin);

	// 判断是否同阵营
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsNotFriend(AActor* FirstActor, AActor* SecondActor);
};

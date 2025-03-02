// Fill out your copyright notice in the Description page of Project Settings.


#include "AuraAssetManager.h"
#include "AuraGameplayTags.h"
#include "AbilitySystemGlobals.h"

UAuraAssetManager& UAuraAssetManager::Get()
{
	// TODO: insert return statement here

	check(GEngine);


	UAuraAssetManager* AuraAssetManager = Cast<UAuraAssetManager>(GEngine->AssetManager);

	return *AuraAssetManager;
}

void UAuraAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();


	// 调用自定义Gamplaytag结构的初始化
	FAuraGameplayTags::InitializeNativeGameplayTags();

	// AS系统初始化全局变量 --- TargetData必须有这一步
	UAbilitySystemGlobals::Get().InitGlobalData();
}

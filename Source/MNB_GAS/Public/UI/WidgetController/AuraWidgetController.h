// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AuraWidgetController.generated.h"

class UAuraAttributeSet;
class UAttributeSet;
class UAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
	GENERATED_BODY()

	FWidgetControllerParams() {}
	// 需要指定指针成员变量的构造方式
	FWidgetControllerParams(
		APlayerController* PC,
		APlayerState* PS,
		UAbilitySystemComponent* ASC,
		UAttributeSet* AS)
		:	PlayerController(PC),
			PlayerState(PS),
			AbilitySystemComponent(ASC),
			AttributeSet(AS)
	{};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerState> PlayerState = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAttributeSet> AttributeSet = nullptr;
};
/**
 * 
 */
UCLASS()
class MNB_GAS_API UAuraWidgetController : public UObject
{
	GENERATED_BODY()
public:
	// 蓝图可调用
	UFUNCTION(BlueprintCallable)
	void SetWidgetControllerParams(const FWidgetControllerParams WCParams);

	// 广播初始化值
	virtual void BroadcastInitialValues();

	// 绑定委托回调
	virtual void BindCallbacksToDependencies();

	
protected:
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<APlayerController> Playercontroller;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<APlayerState> PlayerState;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UAttributeSet> AttributeSet;


};

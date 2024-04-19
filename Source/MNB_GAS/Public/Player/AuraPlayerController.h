// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "AuraPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class IEnemyInterface;
class UAuraInputConfig;
class UAuraAbilitySystemComponent;

class USplineComponent;

/**
 * 
 */
UCLASS()
class MNB_GAS_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AAuraPlayerController();

	virtual void PlayerTick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	// Controller中创建一个指针指向IMC
	UPROPERTY(EditAnywhere, Category = "Input");
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category = "Input");
	TObjectPtr<UInputAction> MoveAction;

	void Move(const FInputActionValue& InputActionValue);

	// 鼠标跟踪
	void CursorTrace();


	IEnemyInterface* LastActor;
	IEnemyInterface* ThisActor;

	FHitResult CursorHit;

	// ABInput三个函数
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	UPROPERTY(EditDefaultsOnly, Category = "Input");
	TObjectPtr<UAuraInputConfig> InputConfig;

	// 添加一个ASC指针
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

	UAuraAbilitySystemComponent* GetAuraASC();


	/// <summary>
	/// 实现鼠标点击角色移动至目标地点
	/// 
	/// </summary>

	// 目标方向
	FVector CachedDestination = FVector::ZeroVector;
	// 跟踪鼠标的事件
	float  FollowTime = 0.f;
	// 短按鼠标的判定界限
	float ShortPressThreshold = 0.5f;
	// 自动运行(自动跟踪的判定量
	bool bAutoRunning = false;
	// 是否正在瞄准某个目标？
	bool bTargeting = false;
	// 自动奔跑的半径范围
	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;
	// 自动奔跑的线条样式
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USplineComponent> Spline;

	// 自动奔跑
	void AutoRun();
};

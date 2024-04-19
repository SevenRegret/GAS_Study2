// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"
#include "Interaction/EnemyInterface.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Input/AuraInputComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/SplineComponent.h"
#include "AuraGameplayTags.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

AAuraPlayerController::AAuraPlayerController()
{
	// 可复制Actor
	bReplicates = true;

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();

	AutoRun();
}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning) return;
	if (APawn* ControllerdPawn = GetPawn())
	{
		const FVector LocationOnSpline =
			Spline->FindLocationClosestToWorldLocation(ControllerdPawn->GetActorLocation(),
				ESplineCoordinateSpace::World);
		const FVector Direction =
			Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);

		ControllerdPawn->AddMovementInput(Direction);
		// 到目标点的距离
		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}

	}
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// begin检查Context
	check(AuraContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}

	//鼠标显示
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	// 设置输入模式细节信息
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 增强组件输入设置为角色自带的InputComponent
	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);


	// move函数绑定
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);

	AuraInputComponent->BindAbilityAction(
		InputConfig,
		this,
		&ThisClass::AbilityInputTagPressed,
		&ThisClass::AbilityInputTagReleased,
		&ThisClass::AbilityInputTagHeld);
}

// 自定义移动函数调用, 结构为InputActionValue
void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	// 轴向量
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	// 旋转两
	const FRotator Rotation = GetControlRotation();
	// 获取yaw的旋转
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	// 计算向前向右
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// 检验pawn的有效
	if (APawn* ControllorPawn = GetPawn<APawn>())
	{
		// 添加向前、向右的移动输入value值。
		ControllorPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControllorPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::CursorTrace()
{
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	// 如果光标阻断
	if (!CursorHit.bBlockingHit) return;

	// 上一帧
	LastActor = ThisActor;
	// 当前帧, 获取到Actor
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());
	
	// 高亮Actor逻辑
	// 如果上一帧指向Acotr为空
	if (LastActor != ThisActor)
	{
		if (LastActor) LastActor->UnHighlightActor();
		if (ThisActor) ThisActor->HighlightActor();
	}
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	// 该Acotr是否有效
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
	// 1.输入进来后，如果时鼠标左键，判断是否自动寻路
		bTargeting = ThisActor ? true : false;
		bAutoRunning = false;
	}
	
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetAuraASC())
		{
			GetAuraASC()->AbilityInputTagReleased(InputTag);
		}
		return;
	}

	// 松开时检测是否有目标
	if (bTargeting)
	{
		if (GetAuraASC())
		{
			GetAuraASC()->AbilityInputTagReleased(InputTag);
		}
	}
	else
	{
		const APawn* ControlledPawn = GetPawn();

		// 如果跟随事件属于短按判定界限内
		if (FollowTime <= ShortPressThreshold && ControlledPawn)
		{
			if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(
				this,
				ControlledPawn->GetActorLocation(),
				CachedDestination))
			{
				Spline->ClearSplinePoints();
				for (const auto& PointLoc : NavPath->PathPoints)
				{
					Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
				}
				CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];
				bAutoRunning = true;
			}
		}
		FollowTime = 0.f;
		bTargeting = false;
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	// 1. 左键寻路检测
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetAuraASC())
		{
			GetAuraASC()->AbilityInputTagHeld(InputTag);
		}
		return;
	}

	if (bTargeting)
	{
		if (GetAuraASC())
		{
			GetAuraASC()->AbilityInputTagHeld(InputTag);
		}
	}
	// 没有瞄准目标
	else
	{
		FollowTime += GetWorld()->GetDeltaSeconds();

		// 命中结果检测, 获取需要追踪的目标
		if (CursorHit.bBlockingHit)
		{
			// 命中点缓存为CachedDestination
			CachedDestination = CursorHit.ImpactPoint;
		}

		// 向着目标出发
		if (APawn* ControlledPawn = GetPawn())
		{
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();

			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}
	
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetAuraASC()
{
	
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = 
			Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}

	return AuraAbilitySystemComponent;
}

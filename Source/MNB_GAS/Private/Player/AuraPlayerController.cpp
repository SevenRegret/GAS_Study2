// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"
#include "Interaction/EnemyInterface.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

AAuraPlayerController::AAuraPlayerController()
{
	// 可复制Actor
	bReplicates = true;
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();

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
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);


	// move函数绑定
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
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
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	// 如果光标阻断
	if (!CursorHit.bBlockingHit) return;

	// 上一帧
	LastActor = ThisActor;
	// 当前帧, 获取到Actor
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());
	
	// 高亮Actor逻辑
	// 如果上一帧指向Acotr为空
	if (LastActor == nullptr)
	{
		// 当前不空
		if (ThisActor != nullptr)
		{
			ThisActor->HighlightActor();
		}
		else
		{
			// 当前也空，啥都不做
		}
	}
	else
	{
		// 上一帧不空，当前帧空
		if (ThisActor == nullptr)
		{
			// 上一帧指向的目标取消高亮
			LastActor->UnHighlightActor();
		}
		else
		{
			// 都有目标但不相同，取消上帧目标，设置当前目标高亮
			if (LastActor != ThisActor)
			{
				LastActor->UnHighlightActor();
				ThisActor->HighlightActor();
			}
		}
	}
}

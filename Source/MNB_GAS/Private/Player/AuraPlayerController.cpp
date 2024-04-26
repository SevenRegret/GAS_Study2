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
#include "GameFramework/Character.h"
#include "UI/Widgets/DamageTextComponent.h"

AAuraPlayerController::AAuraPlayerController()
{
	// �ɸ���Actor
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
		// ��Ŀ���ľ���
		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}

	}
}

void AAuraPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit)
{
	// ֻ�ڷ�����Ƿ�ȥ��ʾ�ı�, ����Ƿ�Ϊ����COntroller
	if (IsValid(TargetCharacter) && DamageTextComponentClass && IsLocalController())
	{
		// �õ�damageText���
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		// ֮ǰһֱ�õ���CreateDefault���ﻻ����ע���������
		DamageText->RegisterComponent();
		// ������֮����Զ����Ŷ�������ͼ���趨�Ĺ��캯������, ������Ա任����ɫ�ܵ��˺���λ��
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		// Ȼ���������룬������������
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		// �����˺�����
		DamageText->SetDamageText(DamageAmount, bBlockedHit, bCriticalHit);

	}
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// begin���Context
	check(AuraContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}

	//�����ʾ
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	// ��������ģʽϸ����Ϣ
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// ��ǿ�����������Ϊ��ɫ�Դ���InputComponent
	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);


	// move������
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);

	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);

	AuraInputComponent->BindAbilityAction(
		InputConfig,
		this,
		&ThisClass::AbilityInputTagPressed,
		&ThisClass::AbilityInputTagReleased,
		&ThisClass::AbilityInputTagHeld);

	
}

// �Զ����ƶ���������, �ṹΪInputActionValue
void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	// ������
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	// ��ת��
	const FRotator Rotation = GetControlRotation();
	// ��ȡyaw����ת
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	// ������ǰ����
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// ����pawn����Ч
	if (APawn* ControllorPawn = GetPawn<APawn>())
	{
		// �����ǰ�����ҵ��ƶ�����valueֵ��
		ControllorPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControllorPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::CursorTrace()
{
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	// ���������
	if (!CursorHit.bBlockingHit) return;

	// ��һ֡
	LastActor = ThisActor;
	// ��ǰ֡, ��ȡ��Actor
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());
	
	// ����Actor�߼�
	// �����һָ֡��AcotrΪ��
	if (LastActor != ThisActor)
	{
		if (LastActor) LastActor->UnHighlightActor();
		if (ThisActor) ThisActor->HighlightActor();
	}
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	// ��Acotr�Ƿ���Ч
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
	// 1.������������ʱ���������ж��Ƿ��Զ�Ѱ·
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

	if (GetAuraASC())
	{
		GetAuraASC()->AbilityInputTagReleased(InputTag);
	}

	// �ɿ�ʱ����Ƿ���Ŀ��
	if (!bTargeting && !bShiftKeyDown)
	{
		const APawn* ControlledPawn = GetPawn();

		// ��������¼����ڶ̰��ж�������
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

				if (NavPath->PathPoints.Num() > 0)
				{
					CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];
					bAutoRunning = true;
				}
			}
		}
		FollowTime = 0.f;
		bTargeting = false;
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	// 1. ���Ѱ·���
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetAuraASC())
		{
			GetAuraASC()->AbilityInputTagHeld(InputTag);
		}
		return;
	}

	if (bTargeting || bShiftKeyDown)
	{
		if (GetAuraASC())
		{
			GetAuraASC()->AbilityInputTagHeld(InputTag);
		}
	}
	// û����׼Ŀ��
	else
	{
		FollowTime += GetWorld()->GetDeltaSeconds();

		// ���н�����, ��ȡ��Ҫ׷�ٵ�Ŀ��
		if (CursorHit.bBlockingHit)
		{
			// ���е㻺��ΪCachedDestination
			CachedDestination = CursorHit.ImpactPoint;
		}

		// ����Ŀ�����
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

// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/AuraHUD.h"

#include "UI/Widgets/AuraUserWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"
#include "AbilitySystemComponent.h"


UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);

		OverlayWidgetController->SetWidgetControllerParams(WCParams);

		OverlayWidgetController->BindCallbacksToDependencies();

	}

	return OverlayWidgetController;
}

UAttributeMenuWidgetController* AAuraHUD::GetUAttributeMenuWidgetController(const FWidgetControllerParams& WCParams)
{

	if (AttributeMenuWidgetController == nullptr)
	{
		AttributeMenuWidgetController = NewObject<UAttributeMenuWidgetController>(this, AttributeMenuWidgetControllerClass);

		AttributeMenuWidgetController->SetWidgetControllerParams(WCParams);

		AttributeMenuWidgetController->BindCallbacksToDependencies();

	}

	return AttributeMenuWidgetController;
}

USpellMenuWidgetController* AAuraHUD::GetSpellMenuWidgetController(const FWidgetControllerParams& WCParams)
{
	if (SpellMenuWidgetController == nullptr)
	{
		SpellMenuWidgetController = NewObject<USpellMenuWidgetController>(this, SpellMenuWidgetControllerClass);

		SpellMenuWidgetController->SetWidgetControllerParams(WCParams);

		SpellMenuWidgetController->BindCallbacksToDependencies();

	}

	return SpellMenuWidgetController;
}

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	// 检测覆层类
	checkf(OverlayWidgetClass, TEXT("Overlay Widget Class uninitialized, please fiil out BP_AuraHUD"));
	checkf(OverlayWidgetControllerClass, TEXT("Overlay Widget Controller Class uninitialized, please fiil out BP_AuraHUD"));

	// 检测成功，使用overlayWIdgetClass创建一个新的widget
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	// 转化为aurawidget
	OverlayWidget = Cast<UAuraUserWidget>(Widget);

	const FWidgetControllerParams WidgetControllerParamas(PC, PS, ASC, AS);
	// 构造参数列表，兵根据参数获取新的overlayController
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParamas);

	OverlayWidget->SetWidgetController(WidgetController);

	// 广播初始值函数
	WidgetController->BroadcastInitialValues();
	Widget->AddToViewport();
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/AuraHUD.h"

#include "UI/Widgets/AuraUserWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystemComponent.h"


UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);

		OverlayWidgetController->SetWidgetControllerParams(WCParams);

		OverlayWidgetController->BindCallbacksToDependencies();

		return OverlayWidgetController;
	}

	return OverlayWidgetController;
}

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	// ��⸲����
	checkf(OverlayWidgetClass, TEXT("Overlay Widget Class uninitialized, please fiil out BP_AuraHUD"));
	checkf(OverlayWidgetControllerClass, TEXT("Overlay Widget Controller Class uninitialized, please fiil out BP_AuraHUD"));

	// ���ɹ���ʹ��overlayWIdgetClass����һ���µ�widget
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	// ת��Ϊaurawidget
	OverlayWidget = Cast<UAuraUserWidget>(Widget);

	const FWidgetControllerParams WidgetControllerParamas(PC, PS, ASC, AS);
	// ��������б��������ݲ�����ȡ�µ�overlayController
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParamas);

	OverlayWidget->SetWidgetController(WidgetController);

	// �㲥��ʼֵ����
	WidgetController->BroadcastInitialValues();
	Widget->AddToViewport();
}
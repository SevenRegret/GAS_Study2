// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "AuraInputConfig.h"
#include "AuraInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class MNB_GAS_API UAuraInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
	
public:

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
	void BindAbilityAction(const UAuraInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc,
		ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc);

};


// 根据输入的资产，为其绑定三个函数
template<class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
void UAuraInputComponent::BindAbilityAction(
	const UAuraInputConfig* InputConfig, 
	UserClass* Object, 
	PressedFuncType PressedFunc,
	ReleasedFuncType ReleasedFunc, 
	HeldFuncType HeldFunc)
{
	check(InputConfig);

	for (const FAuraInputAction Action : InputConfig->AbilityInputAction)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag);
			}

			if (ReleasedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag);
			}

			if (HeldFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, HeldFunc, Action.InputTag);
			}
		}
	}
}

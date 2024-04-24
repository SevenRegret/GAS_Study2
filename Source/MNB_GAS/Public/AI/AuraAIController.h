// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AuraAIController.generated.h"

class UBlackboardComponent;
class UBehaviorTreeComponent;


/**
 * 
 */
UCLASS()
class MNB_GAS_API AAuraAIController : public AAIController
{
	GENERATED_BODY()
public:
	AAuraAIController();


protected:
	// AIController±Ø±¸
	UPROPERTY()
	TObjectPtr<UBlackboardComponent> BlackboardComponent;

	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;
};

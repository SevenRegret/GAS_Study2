// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "AuraCharacter.generated.h"

/**
 * 
 */
UCLASS()
class MNB_GAS_API AAuraCharacter : public AAuraCharacterBase
{
	GENERATED_BODY()
	
public:
	AAuraCharacter();
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

private:
	void InitAbilityInfo();
};
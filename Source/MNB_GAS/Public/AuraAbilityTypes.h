#pragma once

#include "GameplayEffectTypes.h"
#include "AuraAbilityTypes.generated.h"

/*
*	�Զ���GE��������
* 
*/

USTRUCT(BlueprintType)
struct FAuraGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:

	bool IsCriticalHit() const { return bIsCriticalHit; }
	bool IsBlockedHit() const { return bIsBlockedHit; }

	void SetIsCriticalHit(bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }
	void SetIsBlockedHit(bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }

	virtual UScriptStruct* GetScriptStruct() const
	{
		return StaticStruct();
	}

	virtual FAuraGameplayEffectContext* Duplicate() const
	{
		FAuraGameplayEffectContext* NewContext = new FAuraGameplayEffectContext();

		*NewContext = *this;
		if (GetHitResult())
		{
			NewContext->AddHitResult(*GetHitResult(), true);
		}

		return NewContext;
	}

	virtual bool NetSerialize(FArchive& Ar, class UPackageMap*, bool& bOutSuccess);

protected:

	UPROPERTY()
	bool bIsBlockedHit = false;


	UPROPERTY()
	bool bIsCriticalHit = false;
};


template<>
struct TStructOpsTypeTraits<FAuraGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FAuraGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true,
	};
};

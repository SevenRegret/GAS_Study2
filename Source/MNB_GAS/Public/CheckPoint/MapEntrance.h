// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CheckPoint/CheckPoint.h"
#include "MapEntrance.generated.h"

/**
 *  地图入口实现
 */
UCLASS()
class MNB_GAS_API AMapEntrance : public ACheckPoint
{
	GENERATED_BODY()
	

public:
	AMapEntrance(const FObjectInitializer& ObjectInitializer);

	/* Highlight接口*/
	
	virtual void HighlightActor_Implementation() override;
	
	/* Highlight接口*/


	/* Save 接口*/
	virtual void LoadActor_Implementation() override;
	/* Save 接口*/

	// 目标Map
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> DestinationMap;


	// 目标出生点
	UPROPERTY(EditAnywhere)
	FName DestinationPlayerStartTag;

protected:

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult) override;
};

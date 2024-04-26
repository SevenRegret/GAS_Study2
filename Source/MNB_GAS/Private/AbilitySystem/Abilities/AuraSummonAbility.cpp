// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraSummonAbility.h"

TArray<FVector> UAuraSummonAbility::GetSpawnLocations()
{
    // 
    // 获取Actor向前和位置
    const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
    const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
    const float DeltaSpread = SpawnSpread / NumMinions;

    // 每次召唤间隔
    const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector);
    TArray<FVector> SpawnLocations;
    for (int32 i = 0; i < NumMinions; i++)
    {
        const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
        FVector ChosenSpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);

        //  检查随机生成的目标点合法性（上下，是否被占了) 正确的生成在地面而不是空中
        FHitResult Hit;
        GetWorld()->LineTraceSingleByChannel(Hit, ChosenSpawnLocation + FVector(0.f, 0.f, 400.f), ChosenSpawnLocation - FVector(0.f, 0.f, 400.f), ECC_Visibility);
        if (Hit.bBlockingHit)
        {
            ChosenSpawnLocation = Hit.ImpactPoint;
        }
    
        SpawnLocations.Add(ChosenSpawnLocation);
    }

    return SpawnLocations;
}

TSubclassOf<APawn> UAuraSummonAbility::GetRandomMinionClass()
{
    // 可召唤列表里随机
    int32 Selection = FMath::RandRange(0, MinionClasses.Num() - 1);

    return MinionClasses[Selection];
}

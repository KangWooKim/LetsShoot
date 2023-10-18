// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSpawnPoint.h"
#include "Item.h"

AItemSpawnPoint::AItemSpawnPoint()
{
    PrimaryActorTick.bCanEverTick = false; // 이 액터의 틱을 비활성화합니다.
    bReplicates = true; // 네트워크에서 복제를 허용합니다.
}

void AItemSpawnPoint::BeginPlay()
{
    Super::BeginPlay(); // 부모 클래스의 BeginPlay를 호출합니다.
    StartSpawnItemTimer((AActor*)nullptr); // 픽업 스폰 타이머를 시작합니다.
}

void AItemSpawnPoint::SpawnItem()
{
    int32 NumItemClasses = ItemClasses.Num(); // 가능한 픽업 클래스의 수를 얻습니다.
    if (NumItemClasses > 0)
    {
        int32 Selection = FMath::RandRange(0, NumItemClasses - 1); // 무작위 픽업 클래스를 선택합니다.
        SpawnedItem = GetWorld()->SpawnActor<AItem>(ItemClasses[Selection], GetActorTransform()); // 선택한 클래스의 픽업을 이 액터의 위치와 회전으로 스폰합니다.

        if (HasAuthority() && SpawnedItem)
        {
            SpawnedItem->OnDestroyed.AddDynamic(this, &AItemSpawnPoint::StartSpawnItemTimer); // 픽업이 파괴될 때 StartSpawnItemTimer를 호출하도록 설정합니다.
        }
    }
}

void AItemSpawnPoint::SpawnItemTimerFinished()
{
    if (HasAuthority())
    {
        SpawnItem(); // 픽업 스폰 타이머가 끝나면 픽업을 스폰합니다.
    }
}

void AItemSpawnPoint::StartSpawnItemTimer(AActor* DestroyedActor)
{
    const float SpawnTime = FMath::FRandRange(SpawnItemTimeMin, SpawnItemTimeMax); // 최소와 최대 사이에서 무작위 스폰 시간을 선택합니다.
    GetWorldTimerManager().SetTimer(
        spawnItemTimer,
        this,
        &AItemSpawnPoint::SpawnItemTimerFinished,
        SpawnTime // 스폰 시간 후에 SpawnItemTimerFinished를 호출하도록 타이머를 설정합니다.
    );
}

void AItemSpawnPoint::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime); // 부모 클래스의 Tick을 호출합니다.

    // 추가적인 Tick 동작이 필요하면 이곳에 코드를 작성하세요.
}



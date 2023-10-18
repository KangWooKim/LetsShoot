// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSpawnPoint.h"
#include "Item.h"

AItemSpawnPoint::AItemSpawnPoint()
{
    PrimaryActorTick.bCanEverTick = false; // �� ������ ƽ�� ��Ȱ��ȭ�մϴ�.
    bReplicates = true; // ��Ʈ��ũ���� ������ ����մϴ�.
}

void AItemSpawnPoint::BeginPlay()
{
    Super::BeginPlay(); // �θ� Ŭ������ BeginPlay�� ȣ���մϴ�.
    StartSpawnItemTimer((AActor*)nullptr); // �Ⱦ� ���� Ÿ�̸Ӹ� �����մϴ�.
}

void AItemSpawnPoint::SpawnItem()
{
    int32 NumItemClasses = ItemClasses.Num(); // ������ �Ⱦ� Ŭ������ ���� ����ϴ�.
    if (NumItemClasses > 0)
    {
        int32 Selection = FMath::RandRange(0, NumItemClasses - 1); // ������ �Ⱦ� Ŭ������ �����մϴ�.
        SpawnedItem = GetWorld()->SpawnActor<AItem>(ItemClasses[Selection], GetActorTransform()); // ������ Ŭ������ �Ⱦ��� �� ������ ��ġ�� ȸ������ �����մϴ�.

        if (HasAuthority() && SpawnedItem)
        {
            SpawnedItem->OnDestroyed.AddDynamic(this, &AItemSpawnPoint::StartSpawnItemTimer); // �Ⱦ��� �ı��� �� StartSpawnItemTimer�� ȣ���ϵ��� �����մϴ�.
        }
    }
}

void AItemSpawnPoint::SpawnItemTimerFinished()
{
    if (HasAuthority())
    {
        SpawnItem(); // �Ⱦ� ���� Ÿ�̸Ӱ� ������ �Ⱦ��� �����մϴ�.
    }
}

void AItemSpawnPoint::StartSpawnItemTimer(AActor* DestroyedActor)
{
    const float SpawnTime = FMath::FRandRange(SpawnItemTimeMin, SpawnItemTimeMax); // �ּҿ� �ִ� ���̿��� ������ ���� �ð��� �����մϴ�.
    GetWorldTimerManager().SetTimer(
        spawnItemTimer,
        this,
        &AItemSpawnPoint::SpawnItemTimerFinished,
        SpawnTime // ���� �ð� �Ŀ� SpawnItemTimerFinished�� ȣ���ϵ��� Ÿ�̸Ӹ� �����մϴ�.
    );
}

void AItemSpawnPoint::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime); // �θ� Ŭ������ Tick�� ȣ���մϴ�.

    // �߰����� Tick ������ �ʿ��ϸ� �̰��� �ڵ带 �ۼ��ϼ���.
}



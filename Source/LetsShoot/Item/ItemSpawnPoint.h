// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemSpawnPoint.generated.h"

UCLASS()
class LETSSHOOT_API AItemSpawnPoint : public AActor
{
    GENERATED_BODY()

public:
    AItemSpawnPoint(); // ������
    virtual void Tick(float DeltaTime) override; // ƽ �޼���. DeltaTime�� ���� ������ ������ �ð��Դϴ�.

protected:
    virtual void BeginPlay() override; // ������ ���۵� �� ȣ��˴ϴ�.

    UPROPERTY(EditAnywhere) // UE5 �����Ϳ��� ���� �����ϵ��� �մϴ�.
    TArray<TSubclassOf<class AItem>> ItemClasses; // ������ �Ⱦ� Ŭ�������� �����մϴ�.

    UPROPERTY() // �ش� ������ UE5 �ý��ۿ� ���� �����ǵ��� �մϴ�.
    AItem* SpawnedItem; // ������ �Ⱦ��� �����մϴ�.

    void SpawnItem(); // �Ⱦ��� �����ϴ� �޼����Դϴ�.
    void SpawnItemTimerFinished(); // �Ⱦ� ���� Ÿ�̸Ӱ� �Ϸ�� �� ȣ��˴ϴ�.

    UFUNCTION() // UE5 �ý��ۿ��� �� �Լ��� �ν��ϵ��� �մϴ�.
    void StartSpawnItemTimer(AActor* DestroyedActor); // �Ⱦ� ���� Ÿ�̸Ӹ� �����մϴ�. DestroyedActor�� �ı��� ���͸� �����մϴ�.

private:
    FTimerHandle SpawnItemTimer; // �Ⱦ� ���� Ÿ�̸� �ڵ��� �����մϴ�.

    UPROPERTY(EditAnywhere) // UE5 �����Ϳ��� ���� �����ϵ��� �մϴ�.
    float SpawnItemTimeMin; // �Ⱦ� ���� �ּ� �ð��� �����մϴ�.

    UPROPERTY(EditAnywhere) // UE5 �����Ϳ��� ���� �����ϵ��� �մϴ�.
    float SpawnItemTimeMax; // �Ⱦ� ���� �ִ� �ð��� �����մϴ�.

public:
    // �߰����� public ������� �̰��� �߰��ϼ���.

};

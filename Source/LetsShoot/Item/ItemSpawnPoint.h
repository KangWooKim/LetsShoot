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
    AItemSpawnPoint(); // 생성자
    virtual void Tick(float DeltaTime) override; // 틱 메서드. DeltaTime는 이전 프레임 이후의 시간입니다.

protected:
    virtual void BeginPlay() override; // 게임이 시작될 때 호출됩니다.

    UPROPERTY(EditAnywhere) // UE5 에디터에서 수정 가능하도록 합니다.
    TArray<TSubclassOf<class AItem>> ItemClasses; // 가능한 픽업 클래스들을 저장합니다.

    UPROPERTY() // 해당 변수가 UE5 시스템에 의해 추적되도록 합니다.
    AItem* SpawnedItem; // 스폰된 픽업을 참조합니다.

    void SpawnItem(); // 픽업을 스폰하는 메서드입니다.
    void SpawnItemTimerFinished(); // 픽업 스폰 타이머가 완료될 때 호출됩니다.

    UFUNCTION() // UE5 시스템에서 이 함수를 인식하도록 합니다.
    void StartSpawnItemTimer(AActor* DestroyedActor); // 픽업 스폰 타이머를 시작합니다. DestroyedActor는 파괴된 액터를 참조합니다.

private:
    FTimerHandle SpawnItemTimer; // 픽업 스폰 타이머 핸들을 저장합니다.

    UPROPERTY(EditAnywhere) // UE5 에디터에서 수정 가능하도록 합니다.
    float SpawnItemTimeMin; // 픽업 스폰 최소 시간을 저장합니다.

    UPROPERTY(EditAnywhere) // UE5 에디터에서 수정 가능하도록 합니다.
    float SpawnItemTimeMax; // 픽업 스폰 최대 시간을 저장합니다.

public:
    // 추가적인 public 멤버들을 이곳에 추가하세요.

};

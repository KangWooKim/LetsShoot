// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "GetSpeed.generated.h"

/**
 * 
 */
UCLASS()
// AGetSpeed 클래스 정의
class LETSSHOOT_API AGetSpeed : public AItem
{
    GENERATED_BODY()

protected:
    // 오버랩되는 구체에 대한 처리를 담당하는 함수
    virtual void OnSphereOverlap(
        UPrimitiveComponent* OverlappedComponent, // 오버랩이 발생한 기본 컴포넌트
        AActor* OtherActor, // 오버랩이 발생한 다른 액터
        UPrimitiveComponent* OtherComp, // 오버랩이 발생한 다른 컴포넌트
        int32 OtherBodyIndex, // 오버랩이 발생한 다른 컴포넌트의 바디 인덱스
        bool bFromSweep, // 스윕으로부터의 오버랩인지 아닌지를 나타냄
        const FHitResult& SweepResult // 스윕의 결과
    );

private:
    // 에디터에서 수정 가능한 기본 속도 버프
    UPROPERTY(EditAnywhere)
    float BaseSpeedBuff = 1600.f;

    // 에디터에서 수정 가능한 앉은 상태의 속도 버프
    UPROPERTY(EditAnywhere)
    float CrouchSpeedBuff = 850.f;

    // 에디터에서 수정 가능한 속도 버프 지속 시간
    UPROPERTY(EditAnywhere)
    float SpeedBuffTime = 30.f;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "GetShield.generated.h"

/**
 * 
 */
UCLASS()
// LETSSHOOT_API을 통해 DLL 또는 정적 라이브러리의 생성을 위해 사용
// AItem 클래스를 상속받는 AGetShield 클래스
class LETSSHOOT_API AGetShield : public AItem
{
    // Unreal Engine의 클래스 생성 매크로
    GENERATED_BODY()

protected:
    // 오버랩이 발생했을 때 호출되는 함수. AGetShield 클래스에서는 이 함수를 재정의합니다.
    virtual void OnSphereOverlap(
        UPrimitiveComponent* OverlappedComponent,   // 오버랩된 컴포넌트
        AActor* OtherActor,                         // 오버랩한 다른 액터
        UPrimitiveComponent* OtherComp,             // 오버랩한 다른 컴포넌트
        int32 OtherBodyIndex,                       // 오버랩한 바디의 인덱스
        bool bFromSweep,                            // 스윕으로 인해 발생했는지 여부
        const FHitResult& SweepResult               // 스윕 결과
    ) override;

private:
    // 방패를 보충하는 양을 설정하는 변수. EditAnywhere을 통해 에디터에서 수정 가능
    UPROPERTY(EditAnywhere)
    float ShieldReplenishAmount = 100.f;

    // 방패를 보충하는 데 걸리는 시간을 설정하는 변수. EditAnywhere을 통해 에디터에서 수정 가능
    UPROPERTY(EditAnywhere)
    float ShieldReplenishTime = 5.f;
};


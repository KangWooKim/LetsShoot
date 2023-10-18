// Fill out your copyright notice in the Description page of Project Settings.


#include "GetShield.h"
#include "LetsShoot/Character/LetsShootCharacter.h"
#include "LetsShoot/LetsShootComponents/BuffComponent.h"

// AGetShield 클래스의 OnSphereOverlap 함수
void AGetShield::OnSphereOverlap(
    UPrimitiveComponent* OverlappedComponent,   // 오버랩 이벤트가 발생한 컴포넌트
    AActor* OtherActor,                         // 오버랩 이벤트를 발생시킨 다른 액터
    UPrimitiveComponent* OtherComp,             // 오버랩 이벤트를 발생시킨 다른 컴포넌트
    int32 OtherBodyIndex,                       // 오버랩이 발생한 다른 컴포넌트의 바디 인덱스
    bool bFromSweep,                            // 스윕 결과로부터의 오버랩인지 여부
    const FHitResult& SweepResult               // 스윕이 발생했을 때의 결과 정보
)
{
    // 부모 클래스의 오버랩 처리 함수 호출
    Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

    // 오버랩이 발생한 액터가 ALetsShootCharacter 인스턴스인지 확인
    ALetsShootCharacter* LetsShootCharacter = Cast<ALetsShootCharacter>(OtherActor);
    if (LetsShootCharacter)
    {
        // ALetsShootCharacter 인스턴스에서 UBuffComponent를 가져옴
        UBuffComponent* Buff = LetsShootCharacter->GetBuff();
        if (Buff)
        {
            // UBuffComponent의 방패를 보충
            Buff->ReplenishShield(ShieldReplenishAmount, ShieldReplenishTime);
        }
    }

    // 오버랩 처리가 끝나면 자신을 파괴
    Destroy();
}

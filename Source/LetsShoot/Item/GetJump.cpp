// Fill out your copyright notice in the Description page of Project Settings.


#include "GetJump.h"
#include "LetsShoot/Character/LetsShootCharacter.h"
#include "LetsShoot/LetsShootComponents/BuffComponent.h"

void AGetJump::OnSphereOverlap(
    UPrimitiveComponent* OverlappedComponent,  // 이 객체와 겹치는 컴포넌트
    AActor* OtherActor,  // 이 객체와 겹치는 다른 액터
    UPrimitiveComponent* OtherComp,  // 이 객체와 겹치는 다른 컴포넌트
    int32 OtherBodyIndex,  // 겹치는 다른 컴포넌트의 본체 인덱스
    bool bFromSweep,  // 이 겹침이 스윕 동안 발생했는지 여부
    const FHitResult& SweepResult  // 스윕 동안 발생한 첫 충돌에 대한 결과
) {
    // 부모 클래스의 OnSphereOverlap 메서드를 호출합니다.
    Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

    // 겹치는 다른 액터를 ALetsShootCharacter로 변환(캐스팅) 시도합니다.
    ALetsShootCharacter* LetsShootCharacter = Cast<ALetsShootCharacter>(OtherActor);
    // 성공하면 아래 코드를 실행합니다.
    if (LetsShootCharacter)
    {
        // LetsShootCharacter의 Buff 컴포넌트를 얻습니다.
        UBuffComponent* Buff = LetsShootCharacter->GetBuff();
        // Buff 컴포넌트가 있으면 아래 코드를 실행합니다.
        if (Buff)
        {
            // Buff의 점프 능력을 강화합니다.
            Buff->BuffJump(JumpZVelocityBuff, JumpBuffTime);
        }
    }

    // 이 객체를 파괴합니다.
    Destroy();
}

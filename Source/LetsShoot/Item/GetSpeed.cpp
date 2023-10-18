// Fill out your copyright notice in the Description page of Project Settings.


#include "GetSpeed.h"
#include "LetsShoot/Character/LetsShootCharacter.h"
#include "LetsShoot/LetsShootComponents/BuffComponent.h"

// 스피드 픽업의 오버랩이 시작되었을 때 호출되는 메서드
void AGetSpeed::OnSphereOverlap(
    UPrimitiveComponent* OverlappedComponent, // 오버랩된 기본 컴포넌트
    AActor* OtherActor, // 오버랩된 다른 액터
    UPrimitiveComponent* OtherComp, // 오버랩된 다른 컴포넌트
    int32 OtherBodyIndex, // 오버랩된 다른 컴포넌트의 바디 인덱스
    bool bFromSweep, // 스윕으로부터 오버랩이 시작되었는지 여부
    const FHitResult& SweepResult // 스윕의 결과
) {
    // 부모 클래스의 OnSphereOverlap 메서드 호출
    Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

    // OtherActor를 ALetsShootCharacter로 캐스팅
    ALetsShootCharacter* LetsShootCharacter = Cast<ALetsShootCharacter>(OtherActor);
    // 캐스팅에 성공하면
    if (LetsShootCharacter)
    {
        // LetsShootCharacter의 BuffComponent를 얻어옴
        UBuffComponent* Buff = LetsShootCharacter->GetBuff();
        // Buff가 유효하면
        if (Buff)
        {
            // Buff의 속도를 증가시킴
            Buff->BuffSpeed(BaseSpeedBuff, CrouchSpeedBuff, SpeedBuffTime);
        }
    }

    // 스피드 픽업을 파괴
    Destroy();
}

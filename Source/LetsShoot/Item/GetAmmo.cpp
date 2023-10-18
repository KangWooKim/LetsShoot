// Fill out your copyright notice in the Description page of Project Settings.


#include "GetAmmo.h"
#include "LetsShoot/Character/LetsShootCharacter.h"
#include "LetsShoot/LetsShootComponents/CombatComponent.h"

void AGetAmmo::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, // 이 컴포넌트와 충돌이 발생한 컴포넌트
    AActor* OtherActor, // 이 컴포넌트와 충돌한 다른 액터
    UPrimitiveComponent* OtherComp, // 충돌이 발생한 다른 컴포넌트
    int32 OtherBodyIndex, // 충돌한 다른 컴포넌트의 본체 인덱스
    bool bFromSweep, // 이 충돌이 스윕(sweep: 오브젝트가 움직이면서 경로상에 있는 모든 오브젝트와의 충돌을 감지하는 방법)으로부터 발생한 것인지를 나타냄
    const FHitResult& SweepResult) // 스윕 동안 발생한 첫 번째 충돌에 대한 결과
{
    // 부모 클래스의 OnSphereOverlap 메서드를 호출합니다.
    Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

    // OtherActor를 ALetsShootCharacter로 캐스팅합니다.
    // 캐스팅 결과가 유효한 경우에만 아래 로직을 수행합니다.
    ALetsShootCharacter* LetsShootCharacter = Cast<ALetsShootCharacter>(OtherActor);
    if (LetsShootCharacter)
    {
        // LetsShootCharacter의 Combat 컴포넌트를 가져옵니다.
        // Combat 컴포넌트가 유효한 경우에만 아래 로직을 수행합니다.
        UCombatComponent* Combat = LetsShootCharacter->GetCombat();
        if (Combat)
        {
            // Combat 컴포넌트에 탄약을 더해줍니다.
            Combat->PickupAmmo(WeaponType, AmmoAmount);
        }
    }
    // 탄약을 주운 후에는 탄약 픽업 객체를 파괴합니다.
    Destroy();
}

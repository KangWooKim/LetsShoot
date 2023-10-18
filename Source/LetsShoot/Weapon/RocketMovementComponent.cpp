// Fill out your copyright notice in the Description page of Project Settings.


#include "RocketMovementComponent.h"

// URocketMovementComponent 클래스 내의 메서드들을 정의

// HandleBlockingHit 메서드: 차단(Blocking) 히트를 처리
// @param Hit: 차단(Blocking) 히트 결과에 대한 정보를 저장하는 FHitResult 변수
// @param TimeTick: 시간 틱(이동이 실행되는 동안의 시간 간격)을 나타내는 float 변수
// @param MoveDelta: 이동량을 나타내는 FVector 변수
// @param SubTickTimeRemaining: 하위 틱에 남아있는 시간을 나타내는 float 변수에 대한 참조
// @return: EHandleBlockingHitResult 열거형을 반환하며, 다음 하위 단계를 진행하라는 신호를 보냄
URocketMovementComponent::EHandleBlockingHitResult URocketMovementComponent::HandleBlockingHit(const FHitResult& Hit, float TimeTick, const FVector& MoveDelta, float& SubTickTimeRemaining)
{
    Super::HandleBlockingHit(Hit, TimeTick, MoveDelta, SubTickTimeRemaining);
    return EHandleBlockingHitResult::AdvanceNextSubstep;
}

// HandleImpact 메서드: 충돌에 대해 처리
// @param Hit: 충돌 결과에 대한 정보를 저장하는 FHitResult 변수
// @param TimeSlice: 이동의 시간 간격을 나타내는 float 변수
// @param MoveDelta: 이동량을 나타내는 FVector 변수
void URocketMovementComponent::HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta)
{
    // 로켓은 멈추어서는 안 됩니다; 오직 그들의 CollisionBox가 충돌을 감지할 때만 폭발해야 합니다.
}

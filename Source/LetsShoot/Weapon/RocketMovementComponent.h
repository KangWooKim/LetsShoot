// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "RocketMovementComponent.generated.h"

/**
 * 
 */
 // 헤더 파일에서 사용자 정의 클래스인 URocketMovementComponent를 선언하고 기본 클래스인 UProjectileMovementComponent에서 상속받습니다.

UCLASS()
class LETSSHOOT_API URocketMovementComponent : public UProjectileMovementComponent
{
    GENERATED_BODY()

protected:
    // HandleBlockingHit 메서드 오버라이드: 차단(Blocking) 히트를 처리
    // @param Hit: 차단(Blocking) 히트 결과에 대한 정보를 저장하는 FHitResult 변수
    // @param TimeTick: 시간 틱(이동이 실행되는 동안의 시간 간격)을 나타내는 float 변수
    // @param MoveDelta: 이동량을 나타내는 FVector 변수
    // @param SubTickTimeRemaining: 하위 틱에 남아있는 시간을 나타내는 float 변수에 대한 참조
    // @return: EHandleBlockingHitResult 열거형을 반환하며, 다음 하위 단계를 진행하라는 신호를 보냄
    virtual EHandleBlockingHitResult HandleBlockingHit(const FHitResult& Hit, float TimeTick, const FVector& MoveDelta, float& SubTickTimeRemaining) override;

    // HandleImpact 메서드 오버라이드: 충돌에 대해 처리
    // @param Hit: 충돌 결과에 대한 정보를 저장하는 FHitResult 변수
    // @param TimeSlice: 이동의 시간 간격을 나타내는 float 변수, 기본값은 0
    // @param MoveDelta: 이동량을 나타내는 FVector 변수, 기본값은 제로 벡터
    virtual void HandleImpact(const FHitResult& Hit, float TimeSlice = 0.f, const FVector& MoveDelta = FVector::ZeroVector) override;
};

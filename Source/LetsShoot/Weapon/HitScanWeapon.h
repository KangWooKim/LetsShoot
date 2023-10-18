// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "HitScanWeapon.generated.h"

/**
 * 
 */
 // 히트스캔 무기 클래스
UCLASS()
class LETSSHOOT_API AHitScanWeapon : public AWeapon
{
    GENERATED_BODY()

public:
    // 무기 발사 함수를 재정의 합니다.
    // HitTarget: 발사 대상 위치
    virtual void Fire(const FVector& HitTarget) override;

protected:
    // 히트 추적을 수행하는 함수
    // TraceStart: 추적 시작 위치
    // HitTarget: 발사 대상 위치
    // OutHit: 추적 결과
    void WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit);

    // 히트 시 파티클 효과
    UPROPERTY(EditAnywhere)
        class UParticleSystem* ImpactParticles;

    // 히트 시 사운드
    UPROPERTY(EditAnywhere)
        USoundCue* HitSound;

private:
    // 추적 빔 파티클 효과
    UPROPERTY(EditAnywhere)
        UParticleSystem* BeamParticles;

    // 발사구 파티클 효과
    UPROPERTY(EditAnywhere)
        UParticleSystem* MuzzleFlash;

    // 발사 사운드
    UPROPERTY(EditAnywhere)
        USoundCue* FireSound;
};

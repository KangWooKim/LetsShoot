// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileGrenade.generated.h"

/**
 * 
 */
 // UCLASS 매크로를 사용해 Unreal Engine의 오브젝트 시스템에 클래스를 선언
UCLASS()
// AProjectileGrenade 클래스는 AProjectile 클래스를 상속받습니다.
class LETSSHOOT_API AProjectileGrenade : public AProjectile
{
    // 클래스를 생성하고 관리하는 데 필요한 코드를 생성하는 매크로
    GENERATED_BODY()

        // AProjectileGrenade 클래스의 public 접근자
public:
    // AProjectileGrenade 클래스의 기본 생성자
    AProjectileGrenade();

    // 이 클래스가 파괴될 때 호출되는 함수를 오버라이드
    virtual void Destroyed() override;

    // AProjectileGrenade 클래스의 protected 접근자
protected:
    // 플레이가 시작될 때 호출되는 함수를 오버라이드
    virtual void BeginPlay() override;

    // 그레네이드가 바운스할 때 호출되는 함수
    // - ImpactResult : 충돌 결과에 대한 상세 정보
    // - ImpactVelocity : 충돌 시 그레네이드의 속도
    UFUNCTION()
        void OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

    // AProjectileGrenade 클래스의 private 접근자
private:
    // 그레네이드가 바운스할 때 재생되는 사운드를 나타내는 속성
    // 에디터 어디서나 수정 가능
    UPROPERTY(EditAnywhere)
        USoundCue* BounceSound;
};


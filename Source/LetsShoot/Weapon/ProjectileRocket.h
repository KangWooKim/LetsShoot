// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileRocket.generated.h"

/**
 * 
 */
#pragma once

UCLASS()
class LETSSHOOT_API AProjectileRocket : public AProjectile
{
    GENERATED_BODY()

public:
    AProjectileRocket();

    // 파괴될 때 호출되는 메서드입니다.
    virtual void Destroyed() override;

protected:
    // 충돌 시 호출되는 메서드입니다.
    // HitComp: 충돌한 원소의 UPrimitiveComponent
    // OtherActor: 충돌한 다른 액터
    // OtherComp: 충돌한 다른 액터의 UPrimitiveComponent
    // NormalImpulse: 충돌 시 가해진 정상적인 충격
    // Hit: 충돌 결과에 대한 정보
    virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

    // 게임 시작 시 호출되는 메서드입니다.
    virtual void BeginPlay() override;

    // 프로젝타일 루프에 사용되는 사운드 큐
    UPROPERTY(EditAnywhere)
        USoundCue* ProjectileLoop;

    // 프로젝타일 루프에 사용되는 오디오 컴포넌트
    UPROPERTY()
        UAudioComponent* ProjectileLoopComponent;

    // 루프 사운드의 소리 감쇠 설정
    UPROPERTY(EditAnywhere)
        USoundAttenuation* LoopingSoundAttenuation;

    // 로켓 이동 컴포넌트
    UPROPERTY(VisibleAnywhere)
        class URocketMovementComponent* RocketMovementComponent;

private:
};

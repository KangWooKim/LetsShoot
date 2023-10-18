// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileBullet.generated.h"

/**
 * 
 */
 // UCLASS 매크로를 사용해 Unreal Engine의 오브젝트 시스템에 클래스를 선언
UCLASS()
// AProjectileBullet 클래스는 AProjectile 클래스를 상속받습니다.
class LETSSHOOT_API AProjectileBullet : public AProjectile
{
    // 클래스를 생성하고 관리하는 데 필요한 코드를 생성하는 매크로
    GENERATED_BODY()

        // AProjectileBullet 클래스의 public 접근자
public:
    // AProjectileBullet 클래스의 기본 생성자
    AProjectileBullet();

    // 에디터에서 속성이 변경되었을 때 호출되는 함수
#if WITH_EDITOR
    virtual void PostEditChangeProperty(struct FPropertyChangedEvent& Event) override;
#endif

    // AProjectileBullet 클래스의 protected 접근자
protected:
    // 상속받은 클래스의 OnHit 메소드를 오버라이드
    // 이 메소드는 투사체가 대상에 충돌했을 때 호출됨
    // - HitComp : 충돌한 컴포넌트
    // - OtherActor : 충돌한 다른 액터
    // - OtherComp : 충돌한 다른 컴포넌트
    // - NormalImpulse : 충돌에 의해 생성된 정규 충격
    // - Hit : 물리적인 충돌 결과에 대한 정보
    virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

    // 상속받은 클래스의 BeginPlay 메소드를 오버라이드
    // 이 메소드는 플레이가 시작될 때 호출됨
    virtual void BeginPlay() override;
};

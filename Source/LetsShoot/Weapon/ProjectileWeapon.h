// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "ProjectileWeapon.generated.h"

/**
 * 
 */
 // Unreal의 AProjectileWeapon 클래스 선언
UCLASS()
class LETSSHOOT_API AProjectileWeapon : public AWeapon
{
    // Unreal 엔진에 의해 생성된 본문
    GENERATED_BODY()

public:
    // Fire 메서드를 오버라이드. 이 메서드는 발사를 처리함.
    // @param HitTarget: 발사체가 명중해야 하는 위치
    virtual void Fire(const FVector& HitTarget) override;

private:
    // ProjectileClass 변수. 이 변수는 발사될 프로젝타일의 클래스를 저장.
    UPROPERTY(EditAnywhere)
        TSubclassOf<class AProjectile> ProjectileClass;

    // ServerSideRewindProjectileClass 변수. 이 변수는 서버 사이드 리와인드를 사용할 경우 발사될 프로젝타일의 클래스를 저장.
    UPROPERTY(EditAnywhere)
        TSubclassOf<AProjectile> ServerSideRewindProjectileClass;
};

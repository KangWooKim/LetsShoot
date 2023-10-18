// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HitScanWeapon.h"
#include "Shotgun.generated.h"

/**
 * 
 */
U// AShotgun 클래스 선언
UCLASS()
class LETSSHOOT_API AShotgun : public AHitScanWeapon
{
    GENERATED_BODY()

public:
    // 샷건 발사 메서드. 이 메서드는 샷건에서 발사된 펠릿들이 목표로하는 위치들을 매개변수로 받습니다.
    // HitTargets: 샷건에서 발사된 펠릿들이 목표로하는 위치들을 담은 배열입니다.
    virtual void FireShotgun(const TArray<FVector_NetQuantize>& HitTargets);

    // 샷건 트레이스 끝 위치 계산 메서드. 이 메서드는 샷건이 목표로하는 위치와, 샷건에서 발사된 펠릿들이 목표로하는 위치들을 매개변수로 받습니다.
    // HitTarget: 샷건이 목표로 하는 위치입니다.
    // HitTargets: 샷건에서 발사된 펠릿들이 목표로하는 위치들을 담은 배열입니다. 이 메서드에서는 이 배열에 펠릿들이 목표로 하는 위치들을 추가합니다.
    void ShotgunTraceEndWithScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets);

private:
    // 샷건 펠릿 갯수. 이 변수는 에디터에서 설정 가능합니다.
    UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
        uint32 NumberOfPellets = 10;
};

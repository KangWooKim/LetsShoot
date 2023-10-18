// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Flag.generated.h"

/**
 * 
 */
 // 클래스 선언 매크로
UCLASS()
// LetsShoot API의 AFlag 클래스, AWeapon 클래스를 상속합니다.
class LETSSHOOT_API AFlag : public AWeapon
{
    // 바디 생성 매크로
    GENERATED_BODY()

public:
    // AFlag 클래스의 기본 생성자
    AFlag();

    // 드롭되었을 때의 동작을 재정의하는 메서드
    // 부모 클래스 AWeapon의 동일한 메서드를 재정의합니다.
    virtual void Dropped() override;

    // 플래그를 리셋하는 메서드
    void ResetFlag();

protected:
    // 장착되었을 때의 동작을 재정의하는 메서드
    // 부모 클래스 AWeapon의 동일한 메서드를 재정의합니다.
    virtual void OnEquipped() override;

    // 드롭되었을 때의 동작을 재정의하는 메서드
    // 부모 클래스 AWeapon의 동일한 메서드를 재정의합니다.
    virtual void OnDropped() override;

    // 게임이 시작될 때의 동작을 재정의하는 메서드
    // 부모 클래스 AActor의 동일한 메서드를 재정의합니다.
    virtual void BeginPlay() override;

private:
    // 플래그 메쉬를 나타내는 포인터
    // Editor에서 이 변수를 볼 수 있습니다.
    UPROPERTY(VisibleAnywhere)
        UStaticMeshComponent* FlagMesh;

    // 플래그의 초기 변환 값을 저장하는 변수
    FTransform InitialTransform;

public:
    // 초기 변환 값을 가져오는 메서드
    // 반환값 : 플래그의 초기 변환값
    FORCEINLINE FTransform GetInitialTransform() const { return InitialTransform; }
};

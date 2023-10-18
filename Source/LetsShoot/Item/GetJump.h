// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "GetJump.generated.h"

/**
 * 
 */
UCLASS()
// 이 클래스는 AItem 클래스를 상속받아 점프 픽업 아이템을 구현합니다.
class LETSSHOOT_API AGetJump : public AItem
{
	GENERATED_BODY()

protected:
    // 이 메서드는 구현체에서 오버라이드해야 하며, 스피어 컴포넌트와 다른 액터나 컴포넌트가 겹쳤을 때 호출됩니다.
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,  // 이 객체와 겹치는 컴포넌트
		AActor* OtherActor,  // 이 객체와 겹치는 다른 액터
		UPrimitiveComponent* OtherComp,  // 이 객체와 겹치는 다른 컴포넌트
		int32 OtherBodyIndex,  // 겹치는 다른 컴포넌트의 본체 인덱스
		bool bFromSweep,  // 이 겹침이 스윕 동안 발생했는지 여부
		const FHitResult& SweepResult  // 스윕 동안 발생한 첫 충돌에 대한 결과
	);

private:
    // 이 변수는 점프 픽업 아이템을 플레이어가 획득하면 적용될 점프 속도의 증가량을 설정합니다.
	UPROPERTY(EditAnywhere)
	float JumpZVelocityBuff = 4000.f;

    // 이 변수는 점프 속도 증가 효과가 지속될 시간을 설정합니다.
	UPROPERTY(EditAnywhere)
	float JumpBuffTime = 30.f;
};

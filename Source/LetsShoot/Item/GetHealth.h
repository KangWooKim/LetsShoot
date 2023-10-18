// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "GetHealth.generated.h"

/**
 * 
 */
UCLASS()
class LETSSHOOT_API AGetHealth : public AItem
{
	GENERATED_BODY()

public:
	// 기본 생성자입니다. 이 클래스의 객체가 네트워크에서 복제가능하도록 설정합니다.
	AGetHealth();

protected:
	// 이 객체와 다른 객체가 겹치면 호출되는 메서드입니다.
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent, // 이 객체와 겹친 컴포넌트
		AActor* OtherActor, // 이 객체와 겹친 다른 액터
		UPrimitiveComponent* OtherComp, // 이 객체와 겹친 다른 컴포넌트
		int32 OtherBodyIndex, // 겹친 다른 컴포넌트의 본체 인덱스
		bool bFromSweep, // 이 겹침이 스윕으로부터 발생한 것인지를 나타냄
		const FHitResult& SweepResult // 스윕 동안 발생한 첫 번째 충돌에 대한 결과
	);

private:
	// 플레이어의 체력을 얼마나 회복할 지 설정하는 변수입니다. 편집 가능(EditAnywhere)합니다.
	UPROPERTY(EditAnywhere)
		float HealAmount = 100.f;

	// 플레이어가 체력을 얼마나 오랫동안 회복받을 지 설정하는 변수입니다. 편집 가능(EditAnywhere)합니다.
	UPROPERTY(EditAnywhere)
		float HealingTime = 5.f;
};

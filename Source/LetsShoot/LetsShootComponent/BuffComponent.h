// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LETSSHOOT_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// 버프 컴포넌트 생성자
	UBuffComponent();

	// ALetsShootCharacter 클래스가 UBuffComponent의 private 및 protected 멤버에 접근할 수 있도록 친구 클래스로 선언
	friend class ALetsShootCharacter;

	// 체력 회복 메서드 (HealAmount: 회복량, HealingTime: 회복에 걸리는 시간)
	void Heal(float HealAmount, float HealingTime);

	// 방패 채우기 메서드 (ShieldAmount: 채울 방패량, ReplenishTime: 채우는데 걸리는 시간)
	void ReplenishShield(float ShieldAmount, float ReplenishTime);

	// 속도 버프 메서드 (BuffBaseSpeed: 버프를 받은 이후의 기본 속도, BuffCrouchSpeed: 버프를 받은 이후의 앉은 상태의 속도, BuffTime: 버프 지속 시간)
	void BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime);

	// 점프 버프 메서드 (BuffJumpVelocity: 버프를 받은 이후의 점프 속도, BuffTime: 버프 지속 시간)
	void BuffJump(float BuffJumpVelocity, float BuffTime);

	// 초기 속도 설정 메서드 (BaseSpeed: 기본 속도, CrouchSpeed: 앉은 상태의 속도)
	void SetInitialSpeeds(float BaseSpeed, float CrouchSpeed);

	// 초기 점프 속도 설정 메서드 (Velocity: 초기 점프 속도)
	void SetInitialJumpVelocity(float Velocity);

protected:
	// 시작 시 호출되는 메서드
	virtual void BeginPlay() override;

	// 체력 회복 속도 계산 메서드 (DeltaTime: 이전 프레임 이후의 시간)
	void HealRampUp(float DeltaTime);

	// 방패 충전 속도 계산 메서드 (DeltaTime: 이전 프레임 이후의 시간)
	void ShieldRampUp(float DeltaTime);

private:
	// 캐릭터 속성
	UPROPERTY()
		class ALetsShootCharacter* Character;

	// 체력 회복 버프 관련 속성
	bool bHealing = false; // 체력 회복 중인지 여부
	float HealingRate = 0.f; // 체력 회복 속도
	float AmountToHeal = 0.f; // 회복할 체력량

	// 방패 충전 버프 관련 속성
	bool bReplenishingShield = false; // 방패 충전 중인지 여부
	float ShieldReplenishRate = 0.f; // 방패 충전 속도
	float ShieldReplenishAmount = 0.f; // 충전할 방패량

	// 속도 버프 관련 속성
	FTimerHandle SpeedBuffTimer; // 속도 버프 타이머
	void ResetSpeeds(); // 속도 초기화 메서드
	float InitialBaseSpeed; // 초기 기본 속도
	float InitialCrouchSpeed; // 초기 앉은 상태의 속도

	// 네트워크 상의 모든 클라이언트에게 속도 버프 알림 메서드
	UFUNCTION(NetMulticast, Reliable)
		void MulticastSpeedBuff(float BaseSpeed, float CrouchSpeed);

	// 점프 버프 관련 속성
	FTimerHandle JumpBuffTimer; // 점프 버프 타이머
	void ResetJump(); // 점프 초기화 메서드
	float InitialJumpVelocity; // 초기 점프 속도

	// 네트워크 상의 모든 클라이언트에게 점프 버프 알림 메서드
	UFUNCTION(NetMulticast, Reliable)
		void MulticastJumpBuff(float JumpVelocity);

public:
	// 컴포넌트의 주요 틱 함수 (DeltaTime: 이전 프레임 이후의 시간, TickType: 틱 유형, ThisTickFunction: 이 틱 함수)
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};

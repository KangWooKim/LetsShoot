// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffComponent.h"
#include "LetsShoot/Character/LetsShootCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// 생성자
UBuffComponent::UBuffComponent()
{
	PrimaryComponentTick.bCanEverTick = true; // 주 틱 가능 설정
}


// 체력 회복 메서드
// HealAmount: 회복할 체력량
// HealingTime: 체력 회복에 걸릴 시간
void UBuffComponent::Heal(float HealAmount, float HealingTime)
{
	bHealing = true; // 회복 상태로 설정
	HealingRate = HealAmount / HealingTime; // 초당 체력 회복률 설정
	AmountToHeal += HealAmount; // 총 회복할 체력량 설정
}

// 방어막 보충 메서드
// ShieldAmount: 보충할 방어막량
// ReplenishTime: 방어막 보충에 걸릴 시간
void UBuffComponent::ReplenishShield(float ShieldAmount, float ReplenishTime)
{
	bReplenishingShield = true; // 방어막 보충 상태로 설정
	ShieldReplenishRate = ShieldAmount / ReplenishTime; // 초당 방어막 보충률 설정
	ShieldReplenishAmount += ShieldAmount; // 총 보충할 방어막량 설정
}

// 체력을 시간에 따라 서서히 회복시키는 메서드
// DeltaTime: 프레임 간 시간 간격
void UBuffComponent::HealRampUp(float DeltaTime)
{
	if (!bHealing || Character == nullptr || Character->IsElimmed()) return; // 회복 상태가 아니거나 캐릭터가 없거나 사망한 경우 종료

	const float HealThisFrame = HealingRate * DeltaTime; // 이번 프레임에서 회복할 체력량
	Character->SetHealth(FMath::Clamp(Character->GetHealth() + HealThisFrame, 0.f, Character->GetMaxHealth())); // 체력 증가, 최대 체력을 넘지 않도록 클램핑
	Character->UpdateHUDHealth(); // HUD의 체력 표시 갱신
	AmountToHeal -= HealThisFrame; // 남은 회복할 체력량 감소

	if (AmountToHeal <= 0.f || Character->GetHealth() >= Character->GetMaxHealth()) // 모든 체력을 회복하거나 캐릭터의 체력이 최대에 도달한 경우
	{
		bHealing = false; // 회복 상태 해제
		AmountToHeal = 0.f; // 남은 회복할 체력량 초기화
	}
}

// 방어막을 시간에 따라 서서히 보충시키는 메서드
// DeltaTime: 프레임 간 시간 간격
void UBuffComponent::ShieldRampUp(float DeltaTime)
{
	if (!bReplenishingShield || Character == nullptr || Character->IsElimmed()) return; // 방어막 보충 상태가 아니거나 캐릭터가 없거나 사망한 경우 종료

	const float ReplenishThisFrame = ShieldReplenishRate * DeltaTime; // 이번 프레임에서 보충할 방어막량
	Character->SetShield(FMath::Clamp(Character->GetShield() + ReplenishThisFrame, 0.f, Character->GetMaxShield())); // 방어막 증가, 최대 방어막을 넘지 않도록 클램핑
	Character->UpdateHUDShield(); // HUD의 방어막 표시 갱신
	ShieldReplenishAmount -= ReplenishThisFrame; // 남은 보충할 방어막량 감소

	if (ShieldReplenishAmount <= 0.f || Character->GetShield() >= Character->GetMaxShield()) // 모든 방어막을 보충하거나 캐릭터의 방어막이 최대에 도달한 경우
	{
		bReplenishingShield = false; // 방어막 보충 상태 해제
		ShieldReplenishAmount = 0.f; // 남은 보충할 방어막량 초기화
	}
}

// BeginPlay 메서드
void UBuffComponent::BeginPlay()
{
	Super::BeginPlay(); // 부모 클래스의 BeginPlay 호출
}


// 초기 이동 속도 설정 메서드
// BaseSpeed: 기본 이동 속도
// CrouchSpeed: 앉은 상태에서의 이동 속도
void UBuffComponent::SetInitialSpeeds(float BaseSpeed, float CrouchSpeed)
{
	InitialBaseSpeed = BaseSpeed; // 기본 이동 속도 설정
	InitialCrouchSpeed = CrouchSpeed; // 앉은 상태에서의 이동 속도 설정
}

// 초기 점프 속도 설정 메서드
// Velocity: 점프 속도
void UBuffComponent::SetInitialJumpVelocity(float Velocity)
{
	InitialJumpVelocity = Velocity; // 점프 속도 설정
}

// 이동 속도 강화 메서드
// BuffBaseSpeed: 강화된 기본 이동 속도
// BuffCrouchSpeed: 강화된 앉은 상태에서의 이동 속도
// BuffTime: 속도 강화 지속 시간
void UBuffComponent::BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime)
{
	if (Character == nullptr) return; // 캐릭터가 없는 경우 반환

	// BuffTime 후에 ResetSpeeds 메서드 실행을 예약
	Character->GetWorldTimerManager().SetTimer(
		SpeedBuffTimer,
		this,
		&UBuffComponent::ResetSpeeds,
		BuffTime
	);

	if (Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BuffBaseSpeed; // 기본 이동 속도 강화
		Character->GetCharacterMovement()->MaxWalkSpeedCrouched = BuffCrouchSpeed; // 앉은 상태에서의 이동 속도 강화
	}
	MulticastSpeedBuff(BuffBaseSpeed, BuffCrouchSpeed); // 다른 클라이언트들에게도 이동 속도 강화 알림
}

// 이동 속도 초기화 메서드
void UBuffComponent::ResetSpeeds()
{
	if (Character == nullptr || Character->GetCharacterMovement() == nullptr) return; // 캐릭터가 없거나 캐릭터 이동 컴포넌트가 없는 경우 반환

	Character->GetCharacterMovement()->MaxWalkSpeed = InitialBaseSpeed; // 기본 이동 속도 초기화
	Character->GetCharacterMovement()->MaxWalkSpeedCrouched = InitialCrouchSpeed; // 앉은 상태에서의 이동 속도 초기화
	MulticastSpeedBuff(InitialBaseSpeed, InitialCrouchSpeed); // 다른 클라이언트들에게도 이동 속도 초기화 알림
}

// 이동 속도 강화 알림 메서드 (다른 클라이언트들에게 알림)
// BaseSpeed: 기본 이동 속도
// CrouchSpeed: 앉은 상태에서의 이동 속도
void UBuffComponent::MulticastSpeedBuff_Implementation(float BaseSpeed, float CrouchSpeed)
{
	if (Character && Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseSpeed; // 기본 이동 속도 설정
		Character->GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed; // 앉은 상태에서의 이동 속도 설정
	}
}

// 점프 강화 메서드
// BuffJumpVelocity: 강화된 점프 속도
// BuffTime: 점프 강화 지속 시간
void UBuffComponent::BuffJump(float BuffJumpVelocity, float BuffTime)
{
	if (Character == nullptr) return; // 캐릭터가 없는 경우 반환

	// BuffTime 후에 ResetJump 메서드 실행을 예약
	Character->GetWorldTimerManager().SetTimer(
		JumpBuffTimer,
		this,
		&UBuffComponent::ResetJump,
		BuffTime
	);

	if (Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->JumpZVelocity = BuffJumpVelocity; // 점프 속도 강화
	}
	MulticastJumpBuff(BuffJumpVelocity); // 다른 클라이언트들에게도 점프 속도 강화 알림
}

// 점프 강화 알림 메서드 (다른 클라이언트들에게 알림)
// JumpVelocity: 점프 속도
void UBuffComponent::MulticastJumpBuff_Implementation(float JumpVelocity)
{
	if (Character && Character->GetCharacterMovement())
	{
		Character->GetCharacterMovement()->JumpZVelocity = JumpVelocity; // 점프 속도 설정
	}
}


// 점프 초기화 메서드
void UBuffComponent::ResetJump()
{
	if (Character->GetCharacterMovement()) // 캐릭터 이동 컴포넌트가 있는 경우
	{
		Character->GetCharacterMovement()->JumpZVelocity = InitialJumpVelocity; // 점프 속도를 초기 점프 속도로 설정
	}
	MulticastJumpBuff(InitialJumpVelocity); // 다른 클라이언트들에게 점프 속도 초기화 알림
}

// 컴포넌트의 주요 틱 함수
// DeltaTime: 이전 프레임 이후의 시간
// TickType: 틱 유형
// ThisTickFunction: 이 틱 함수
void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction); // 부모 클래스의 TickComponent 호출

	HealRampUp(DeltaTime); // 체력 회복 속도 계산
	ShieldRampUp(DeltaTime); // 방패 충전 속도 계산
}


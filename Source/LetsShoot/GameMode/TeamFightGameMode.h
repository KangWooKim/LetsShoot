// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LetsShootGameMode.h"
#include "TeamFightGameMode.generated.h"

/**
 * 
 */
UCLASS()
// 팀 게임 모드를 나타내는 클래스
class LETSSHOOT_API ATeamFightGameMode : public ALetsShootGameMode
{
	GENERATED_BODY()
public:
	// 생성자
	ATeamFightGameMode();

	// 새로운 플레이어가 로그인했을 때 호출되는 메서드
	// NewPlayer : 로그인한 플레이어 컨트롤러
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// 플레이어가 로그아웃했을 때 호출되는 메서드
	// Exiting : 로그아웃하는 컨트롤러
	virtual void Logout(AController* Exiting) override;

	// 데미지를 계산하는 메서드
	// Attacker : 공격자 컨트롤러
	// Victim : 피해자 컨트롤러
	// BaseDamage : 기본 데미지 값
	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage) override;

	// 플레이어가 제거되었을 때 호출되는 메서드
	// ElimmedCharacter : 제거된 캐릭터
	// VictimController : 피해자의 플레이어 컨트롤러
	// AttackerController : 공격자의 플레이어 컨트롤러
	virtual void PlayerEliminated(class ALetsShootCharacter* ElimmedCharacter, class ALetsShootPlayerController* VictimController, ALetsShootPlayerController* AttackerController) override;

protected:
	// 경기가 시작되었을 때 호출되는 메서드
	virtual void HandleMatchHasStarted() override;
};


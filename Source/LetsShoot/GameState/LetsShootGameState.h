// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "LetsShootGameState.generated.h"

/**
 * 
 */
UCLASS()
// LetsShoot 게임 상태 클래스
class LETSSHOOT_API ALetsShootGameState : public AGameState
{
	GENERATED_BODY()
public:
	// 복제되는 속성들을 설정하는 메서드
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 최고 점수를 갱신하는 메서드
	// ScoringPlayer : 점수를 올린 플레이어 상태
	void UpdateTopScore(class ALetsShootPlayerState* ScoringPlayer);

	// 복제되는 속성, 최고 점수를 얻은 플레이어 리스트
	UPROPERTY(Replicated)
		TArray<ALetsShootPlayerState*> TopScoringPlayers;

	/**
	* 팀 관련 메서드와 변수
	*/

	// 레드팀이 점수를 얻었을 때 호출되는 메서드
	void RedTeamScores();

	// 블루팀이 점수를 얻었을 때 호출되는 메서드
	void BlueTeamScores();

	// 레드팀 플레이어 리스트
	TArray<ALetsShootPlayerState*> RedTeam;

	// 블루팀 플레이어 리스트
	TArray<ALetsShootPlayerState*> BlueTeam;

	// 복제되는 속성, 레드팀의 현재 점수
	UPROPERTY(ReplicatedUsing = OnRep_RedTeamScore)
		float RedTeamScore = 0.f;

	// 복제되는 속성, 블루팀의 현재 점수
	UPROPERTY(ReplicatedUsing = OnRep_BlueTeamScore)
		float BlueTeamScore = 0.f;

	// 레드팀 점수가 복제되었을 때 호출되는 메서드
	UFUNCTION()
		void OnRep_RedTeamScore();

	// 블루팀 점수가 복제되었을 때 호출되는 메서드
	UFUNCTION()
		void OnRep_BlueTeamScore();

private:
	// 현재 최고 점수
	float TopScore = 0.f;
};


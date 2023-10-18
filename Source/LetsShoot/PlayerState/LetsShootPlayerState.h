// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "LetsShoot/LetsShootTypes/Team.h"
#include "LetsShootPlayerState.generated.h"

/**
 * 
 */
 // 플레이어의 상태를 나타내는 클래스
UCLASS()
class LETSSHOOT_API ALetsShootPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	// 라이프타임 동안 복제해야하는 속성을 가져옵니다. 
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	/**
	* 복제 알림
	*/
	// 점수가 복제될 때 호출되는 함수
	virtual void OnRep_Score() override;

	// 패배가 복제될 때 호출되는 함수
	UFUNCTION()
		virtual void OnRep_Defeats();

	// 점수를 추가하는 함수, ScoreAmount - 추가할 점수의 양
	void AddToScore(float ScoreAmount);

	// 패배를 추가하는 함수, DefeatsAmount - 추가할 패배의 양
	void AddToDefeats(int32 DefeatsAmount);

private:
	// 플레이어 캐릭터의 참조
	UPROPERTY()
		class ALetsShootCharacter* Character;

	// 플레이어 컨트롤러의 참조
	UPROPERTY()
		class ALetsShootPlayerController* Controller;

	// 패배의 수, 복제 시 OnRep_Defeats가 호출됨
	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
		int32 Defeats;

	// 플레이어의 팀, 복제 시 OnRep_Team이 호출됨
	UPROPERTY(ReplicatedUsing = OnRep_Team)
		ETeamColor Team = ETeam::ET_NoTeam;

	// 팀이 복제될 때 호출되는 함수
	UFUNCTION()
		void OnRep_Team();

public:
	// 팀을 가져오는 인라인 함수, 반환값: 현재 팀
	FORCEINLINE ETeamColor GetTeam() const { return Team; }

	// 팀을 설정하는 함수, TeamToSet - 설정할 팀
	void SetTeam(ETeamColor TeamToSet);
};

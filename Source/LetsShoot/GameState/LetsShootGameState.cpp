// Fill out your copyright notice in the Description page of Project Settings.


#include "LetsShootGameState.h"
#include "Net/UnrealNetwork.h"
#include "LetsShoot/PlayerState/LetsShootPlayerState.h"
#include "LetsShoot/PlayerController/LetsShootPlayerController.h"

// 생명주기 동안 복제되는 속성을 관리하는 메서드
void ALetsShootGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 복제할 속성 지정
	DOREPLIFETIME(ALetsShootGameState, TopScoringPlayers);
	DOREPLIFETIME(ALetsShootGameState, RedTeamScore);
	DOREPLIFETIME(ALetsShootGameState, BlueTeamScore);
}

// 최고 점수를 갱신하는 메서드
// ScoringPlayer : 점수를 올린 플레이어 상태
void ALetsShootGameState::UpdateTopScore(class ALetsShootPlayerState* ScoringPlayer)
{
	// 최고 점수를 달성한 플레이어를 관리하는 로직
	if (TopScoringPlayers.Num() == 0)
	{
		TopScoringPlayers.Add(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
	else if (ScoringPlayer->GetScore() == TopScore)
	{
		TopScoringPlayers.AddUnique(ScoringPlayer);
	}
	else if (ScoringPlayer->GetScore() > TopScore)
	{
		TopScoringPlayers.Empty();
		TopScoringPlayers.AddUnique(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
}

// 레드팀이 점수를 얻었을 때 호출되는 메서드
void ALetsShootGameState::RedTeamScores()
{
	++RedTeamScore; // 레드팀 점수 증가
	ALetsShootPlayerController* BPlayer = Cast<ALetsShootPlayerController>(GetWorld()->GetFirstPlayerController());
	if (BPlayer)
	{
		BPlayer->SetHUDRedTeamScore(RedTeamScore); // HUD에 레드팀 점수 갱신
	}
}

// 블루팀이 점수를 얻었을 때 호출되는 메서드
void ALetsShootGameState::BlueTeamScores()
{
	++BlueTeamScore; // 블루팀 점수 증가
	ALetsShootPlayerController* BPlayer = Cast<ALetsShootPlayerController>(GetWorld()->GetFirstPlayerController());
	if (BPlayer)
	{
		BPlayer->SetHUDBlueTeamScore(BlueTeamScore); // HUD에 블루팀 점수 갱신
	}
}

// 레드팀 점수가 복제되었을 때 호출되는 메서드
void ALetsShootGameState::OnRep_RedTeamScore()
{
	ALetsShootPlayerController* BPlayer = Cast<ALetsShootPlayerController>(GetWorld()->GetFirstPlayerController());
	if (BPlayer)
	{
		BPlayer->SetHUDRedTeamScore(RedTeamScore); // HUD에 레드팀 점수 갱신
	}
}

// 블루팀 점수가 복제되었을 때 호출되는 메서드
void ALetsShootGameState::OnRep_BlueTeamScore()
{
	ALetsShootPlayerController* BPlayer = Cast<ALetsShootPlayerController>(GetWorld()->GetFirstPlayerController());
	if (BPlayer)
	{
		BPlayer->SetHUDBlueTeamScore(BlueTeamScore); // HUD에 블루팀 점수 갱신
	}
}


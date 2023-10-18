// Fill out your copyright notice in the Description page of Project Settings.


#include "LetsShootGameState.h"
#include "Net/UnrealNetwork.h"
#include "LetsShoot/PlayerState/LetsShootPlayerState.h"
#include "LetsShoot/PlayerController/LetsShootPlayerController.h"

// �����ֱ� ���� �����Ǵ� �Ӽ��� �����ϴ� �޼���
void ALetsShootGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// ������ �Ӽ� ����
	DOREPLIFETIME(ALetsShootGameState, TopScoringPlayers);
	DOREPLIFETIME(ALetsShootGameState, RedTeamScore);
	DOREPLIFETIME(ALetsShootGameState, BlueTeamScore);
}

// �ְ� ������ �����ϴ� �޼���
// ScoringPlayer : ������ �ø� �÷��̾� ����
void ALetsShootGameState::UpdateTopScore(class ALetsShootPlayerState* ScoringPlayer)
{
	// �ְ� ������ �޼��� �÷��̾ �����ϴ� ����
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

// �������� ������ ����� �� ȣ��Ǵ� �޼���
void ALetsShootGameState::RedTeamScores()
{
	++RedTeamScore; // ������ ���� ����
	ALetsShootPlayerController* BPlayer = Cast<ALetsShootPlayerController>(GetWorld()->GetFirstPlayerController());
	if (BPlayer)
	{
		BPlayer->SetHUDRedTeamScore(RedTeamScore); // HUD�� ������ ���� ����
	}
}

// ������� ������ ����� �� ȣ��Ǵ� �޼���
void ALetsShootGameState::BlueTeamScores()
{
	++BlueTeamScore; // ����� ���� ����
	ALetsShootPlayerController* BPlayer = Cast<ALetsShootPlayerController>(GetWorld()->GetFirstPlayerController());
	if (BPlayer)
	{
		BPlayer->SetHUDBlueTeamScore(BlueTeamScore); // HUD�� ����� ���� ����
	}
}

// ������ ������ �����Ǿ��� �� ȣ��Ǵ� �޼���
void ALetsShootGameState::OnRep_RedTeamScore()
{
	ALetsShootPlayerController* BPlayer = Cast<ALetsShootPlayerController>(GetWorld()->GetFirstPlayerController());
	if (BPlayer)
	{
		BPlayer->SetHUDRedTeamScore(RedTeamScore); // HUD�� ������ ���� ����
	}
}

// ����� ������ �����Ǿ��� �� ȣ��Ǵ� �޼���
void ALetsShootGameState::OnRep_BlueTeamScore()
{
	ALetsShootPlayerController* BPlayer = Cast<ALetsShootPlayerController>(GetWorld()->GetFirstPlayerController());
	if (BPlayer)
	{
		BPlayer->SetHUDBlueTeamScore(BlueTeamScore); // HUD�� ����� ���� ����
	}
}


// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "LetsShootGameState.generated.h"

/**
 * 
 */
UCLASS()
// LetsShoot ���� ���� Ŭ����
class LETSSHOOT_API ALetsShootGameState : public AGameState
{
	GENERATED_BODY()
public:
	// �����Ǵ� �Ӽ����� �����ϴ� �޼���
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// �ְ� ������ �����ϴ� �޼���
	// ScoringPlayer : ������ �ø� �÷��̾� ����
	void UpdateTopScore(class ALetsShootPlayerState* ScoringPlayer);

	// �����Ǵ� �Ӽ�, �ְ� ������ ���� �÷��̾� ����Ʈ
	UPROPERTY(Replicated)
		TArray<ALetsShootPlayerState*> TopScoringPlayers;

	/**
	* �� ���� �޼���� ����
	*/

	// �������� ������ ����� �� ȣ��Ǵ� �޼���
	void RedTeamScores();

	// ������� ������ ����� �� ȣ��Ǵ� �޼���
	void BlueTeamScores();

	// ������ �÷��̾� ����Ʈ
	TArray<ALetsShootPlayerState*> RedTeam;

	// ����� �÷��̾� ����Ʈ
	TArray<ALetsShootPlayerState*> BlueTeam;

	// �����Ǵ� �Ӽ�, �������� ���� ����
	UPROPERTY(ReplicatedUsing = OnRep_RedTeamScore)
		float RedTeamScore = 0.f;

	// �����Ǵ� �Ӽ�, ������� ���� ����
	UPROPERTY(ReplicatedUsing = OnRep_BlueTeamScore)
		float BlueTeamScore = 0.f;

	// ������ ������ �����Ǿ��� �� ȣ��Ǵ� �޼���
	UFUNCTION()
		void OnRep_RedTeamScore();

	// ����� ������ �����Ǿ��� �� ȣ��Ǵ� �޼���
	UFUNCTION()
		void OnRep_BlueTeamScore();

private:
	// ���� �ְ� ����
	float TopScore = 0.f;
};


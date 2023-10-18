// Fill out your copyright notice in the Description page of Project Settings.


#include "GetFlagGameMode.h"
#include "LetsShoot/Weapon/Flag.h"
#include "LetsShoot/CaptureTheFlag/FlagZone.h"
#include "LetsShoot/GameState/LetsShootGameState.h"

// �÷��̾ ���ŵ� �� ȣ��Ǵ� �޼���
// ElimmedCharacter: ���ŵ� ĳ���Ϳ� ���� ����
// VictimController: ����� �÷��̾��� ��Ʈ�ѷ��� ���� ����
// AttackerController: ������ �÷��̾��� ��Ʈ�ѷ��� ���� ����
void AGetFlagGameMode::PlayerEliminated(class ALetsShootCharacter* ElimmedCharacter, class ALetsShootPlayerController* VictimController, ALetsShootPlayerController* AttackerController)
{
	// �⺻ ���� ����� PlayerEliminated �Լ��� ȣ���Ͽ� �⺻ ������ �����մϴ�.
	ALetsShootGameMode::PlayerEliminated(ElimmedCharacter, VictimController, AttackerController);
}

// ����� ĸó�� �� ȣ��Ǵ� �޼���
// Flag: ĸó�� ��߿� ���� ����
// Zone: ����� ĸó�� ���� ���� ����
void AGetFlagGameMode::FlagCaptured(AFlag* Flag, AFlagZone* Zone)
{
	// ����� ���� ���� ���� �ٸ��ٸ� ��ȿ�� ĸó�� �����մϴ�.
	bool bValidCapture = Flag->GetTeam() != Zone->Team;

	// ���� ���� ���¸� �����ɴϴ�.
	ALetsShootGameState* BGameState = Cast<ALetsShootGameState>(GameState);

	// ���� ���°� ��ȿ�� ���
	if (BGameState)
	{
		// ���� ���� �Ķ����� ���
		if (Zone->Team == ETeam::ET_BlueTeam)
		{
			// �Ķ����� ������ �ο��մϴ�.
			BGameState->BlueTeamScores();
		}
		// ���� ���� �������� ���
		if (Zone->Team == ETeam::ET_RedTeam)
		{
			// �������� ������ �ο��մϴ�.
			BGameState->RedTeamScores();
		}
	}
}


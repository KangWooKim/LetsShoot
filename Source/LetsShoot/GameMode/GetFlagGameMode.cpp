// Fill out your copyright notice in the Description page of Project Settings.


#include "GetFlagGameMode.h"
#include "LetsShoot/Weapon/Flag.h"
#include "LetsShoot/CaptureTheFlag/FlagZone.h"
#include "LetsShoot/GameState/LetsShootGameState.h"

// 플레이어가 제거될 때 호출되는 메서드
// ElimmedCharacter: 제거된 캐릭터에 대한 참조
// VictimController: 희생된 플레이어의 컨트롤러에 대한 참조
// AttackerController: 공격한 플레이어의 컨트롤러에 대한 참조
void AGetFlagGameMode::PlayerEliminated(class ALetsShootCharacter* ElimmedCharacter, class ALetsShootPlayerController* VictimController, ALetsShootPlayerController* AttackerController)
{
	// 기본 게임 모드의 PlayerEliminated 함수를 호출하여 기본 로직을 수행합니다.
	ALetsShootGameMode::PlayerEliminated(ElimmedCharacter, VictimController, AttackerController);
}

// 깃발이 캡처될 때 호출되는 메서드
// Flag: 캡처된 깃발에 대한 참조
// Zone: 깃발이 캡처된 존에 대한 참조
void AGetFlagGameMode::FlagCaptured(AFlag* Flag, AFlagZone* Zone)
{
	// 깃발의 팀과 존의 팀이 다르다면 유효한 캡처로 간주합니다.
	bool bValidCapture = Flag->GetTeam() != Zone->Team;

	// 현재 게임 상태를 가져옵니다.
	ALetsShootGameState* BGameState = Cast<ALetsShootGameState>(GameState);

	// 게임 상태가 유효한 경우
	if (BGameState)
	{
		// 존의 팀이 파란팀인 경우
		if (Zone->Team == ETeam::ET_BlueTeam)
		{
			// 파란팀에 점수를 부여합니다.
			BGameState->BlueTeamScores();
		}
		// 존의 팀이 빨간팀인 경우
		if (Zone->Team == ETeam::ET_RedTeam)
		{
			// 빨간팀에 점수를 부여합니다.
			BGameState->RedTeamScores();
		}
	}
}


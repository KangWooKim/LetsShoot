// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamFightGameMode.h"
#include "LetsShoot/GameState/LetsShootGameState.h"
#include "LetsShoot/PlayerState/LetsShootPlayerState.h"
#include "LetsShoot/PlayerController/LetsShootPlayerController.h"
#include "Kismet/GameplayStatics.h"

// ATeamFightGameMode의 생성자
ATeamFightGameMode::ATeamFightGameMode()
{
	// 팀 매치를 true로 설정합니다.
	bTeamsMatch = true;
}

// 새로운 플레이어가 로그인했을 때 호출되는 메서드
void ATeamFightGameMode::PostLogin(APlayerController* NewPlayer)
{
	// 부모 클래스의 PostLogin 메서드 호출
	Super::PostLogin(NewPlayer);

	// 게임 상태를 가져오고, ALetsShootGameState로 캐스팅합니다.
	ALetsShootGameState* BGameState = Cast<ALetsShootGameState>(UGameplayStatics::GetGameState(this));
	if (BGameState)
	{
		// 플레이어 상태를 가져오고, ALetsShootPlayerState로 캐스팅합니다.
		ALetsShootPlayerState* BPState = NewPlayer->GetPlayerState<ALetsShootPlayerState>();

		// 플레이어 상태가 유효하고, 팀이 설정되지 않은 경우
		if (BPState && BPState->GetTeam() == ETeam::ET_NoTeam)
		{
			// 블루팀의 인원 수가 레드팀보다 많으면 레드팀에 추가
			if (BGameState->BlueTeam.Num() >= BGameState->RedTeam.Num())
			{
				BGameState->RedTeam.AddUnique(BPState);
				BPState->SetTeam(ETeam::ET_RedTeam);
			}
			// 그렇지 않으면 블루팀에 추가
			else
			{
				BGameState->BlueTeam.AddUnique(BPState);
				BPState->SetTeam(ETeam::ET_BlueTeam);
			}
		}
	}
}

// 플레이어가 로그아웃할 때 호출되는 메서드
void ATeamFightGameMode::Logout(AController* Exiting)
{
	// 게임 상태를 가져오고, ALetsShootGameState로 캐스팅합니다.
	ALetsShootGameState* BGameState = Cast<ALetsShootGameState>(UGameplayStatics::GetGameState(this));

	// 플레이어 상태를 가져오고, ALetsShootPlayerState로 캐스팅합니다.
	ALetsShootPlayerState* BPState = Exiting->GetPlayerState<ALetsShootPlayerState>();
	if (BGameState && BPState)
	{
		// 레드팀에 있는 경우, 레드팀에서 제거
		if (BGameState->RedTeam.Contains(BPState))
		{
			BGameState->RedTeam.Remove(BPState);
		}
		// 블루팀에 있는 경우, 블루팀에서 제거
		if (BGameState->BlueTeam.Contains(BPState))
		{
			BGameState->BlueTeam.Remove(BPState);
		}
	}
}


// 매치가 시작되었을 때 호출되는 메서드
void ATeamFightGameMode::HandleMatchHasStarted()
{
	// 부모 클래스의 HandleMatchHasStarted 메서드 호출
	Super::HandleMatchHasStarted();

	// 게임 상태를 가져오고, ALetsShootGameState로 캐스팅합니다.
	ALetsShootGameState* BGameState = Cast<ALetsShootGameState>(UGameplayStatics::GetGameState(this));
	if (BGameState)
	{
		// 플레이어 배열을 순회합니다.
		for (auto PState : BGameState->PlayerArray)
		{
			// 플레이어 상태를 가져오고, ALetsShootPlayerState로 캐스팅합니다.
			ALetsShootPlayerState* BPState = Cast<ALetsShootPlayerState>(PState.Get());

			// 플레이어 상태가 유효하고, 팀이 설정되지 않은 경우
			if (BPState && BPState->GetTeam() == ETeam::ET_NoTeam)
			{
				// 블루팀의 인원 수가 레드팀보다 많으면 레드팀에 추가
				if (BGameState->BlueTeam.Num() >= BGameState->RedTeam.Num())
				{
					BGameState->RedTeam.AddUnique(BPState);
					BPState->SetTeam(ETeam::ET_RedTeam);
				}
				// 그렇지 않으면 블루팀에 추가
				else
				{
					BGameState->BlueTeam.AddUnique(BPState);
					BPState->SetTeam(ETeam::ET_BlueTeam);
				}
			}
		}
	}
}

// 공격자가 피해자에게 가할 데미지를 계산하는 메서드
// Attacker : 공격자의 컨트롤러
// Victim : 피해자의 컨트롤러
// BaseDamage : 기본 데미지 값
float ATeamFightGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	// 공격자와 피해자의 플레이어 상태를 가져옵니다.
	ALetsShootPlayerState* AttackerPState = Attacker->GetPlayerState<ALetsShootPlayerState>();
	ALetsShootPlayerState* VictimPState = Victim->GetPlayerState<ALetsShootPlayerState>();

	// 공격자나 피해자의 상태가 유효하지 않다면, 기본 데미지를 반환합니다.
	if (AttackerPState == nullptr || VictimPState == nullptr) return BaseDamage;

	// 자기 자신을 공격하는 경우, 기본 데미지를 반환합니다.
	if (VictimPState == AttackerPState)
	{
		return BaseDamage;
	}

	// 같은 팀의 플레이어를 공격하는 경우, 데미지는 0입니다.
	if (AttackerPState->GetTeam() == VictimPState->GetTeam())
	{
		return 0.f;
	}

	// 그렇지 않은 경우, 기본 데미지를 반환합니다.
	return BaseDamage;
}


// 플레이어가 제거됐을 때 호출되는 메서드
// ElimmedCharacter : 제거된 캐릭터
// VictimController : 피해자의 플레이어 컨트롤러
// AttackerController : 공격자의 플레이어 컨트롤러
void ATeamFightGameMode::PlayerEliminated(ALetsShootCharacter* ElimmedCharacter, ALetsShootPlayerController* VictimController, ALetsShootPlayerController* AttackerController)
{
	// 부모 클래스의 PlayerEliminated 메서드 호출
	Super::PlayerEliminated(ElimmedCharacter, VictimController, AttackerController);

	// 게임 상태를 가져오고, ALetsShootGameState로 캐스팅합니다.
	ALetsShootGameState* BGameState = Cast<ALetsShootGameState>(UGameplayStatics::GetGameState(this));

	// 공격자의 플레이어 상태를 가져옵니다.
	ALetsShootPlayerState* AttackerPlayerState = AttackerController ? Cast<ALetsShootPlayerState>(AttackerController->PlayerState) : nullptr;

	// 게임 상태와 공격자의 플레이어 상태가 유효한 경우
	if (BGameState && AttackerPlayerState)
	{
		// 공격자가 블루팀이라면, 블루팀의 점수를 올립니다.
		if (AttackerPlayerState->GetTeam() == ETeam::ET_BlueTeam)
		{
			BGameState->BlueTeamScores();
		}
		// 공격자가 레드팀이라면, 레드팀의 점수를 올립니다.
		if (AttackerPlayerState->GetTeam() == ETeam::ET_RedTeam)
		{
			BGameState->RedTeamScores();
		}
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "LetsShootGameMode.h"
#include "LetsShoot/Character/LetsShootCharacter.h"
#include "LetsShoot/PlayerController/LetsShootPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "LetsShoot/PlayerState/LetsShootPlayerState.h"
#include "LetsShoot/GameState/LetsShootGameState.h"

// 미리 정의된 매치 상태의 이름을 지정합니다.
namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}

// ALetsShootGameMode의 생성자
ALetsShootGameMode::ALetsShootGameMode()
{
	bDelayedStart = true; // 게임이 시작되기 전에 딜레이를 적용합니다.
}

// BeginPlay 메서드. 게임이 시작되면 호출됩니다.
void ALetsShootGameMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartingTime = GetWorld()->GetTimeSeconds(); // 게임 시작 시간을 기록합니다.
}

// Tick 메서드. 프레임마다 호출됩니다.
// @param DeltaTime: 이전 프레임과의 시간 차이
void ALetsShootGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MatchState == MatchState::WaitingToStart) // 매치가 시작하기를 기다리는 중이라면
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime; // 카운트다운 시간을 업데이트합니다.
		if (CountdownTime <= 0.f) // 카운트다운이 끝나면
		{
			StartMatch(); // 매치를 시작합니다.
		}
	}
	else if (MatchState == MatchState::InProgress) // 매치가 진행 중이라면
	{
		CountdownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime; // 카운트다운 시간을 업데이트합니다.
		if (CountdownTime <= 0.f) // 매치 시간이 끝나면
		{
			SetMatchState(MatchState::Cooldown); // 매치 상태를 Cooldown으로 설정합니다.
		}
	}
	else if (MatchState == MatchState::Cooldown) // 쿨다운 중이라면
	{
		CountdownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime; // 카운트다운 시간을 업데이트합니다.
		if (CountdownTime <= 0.f) // 쿨다운이 끝나면
		{
			RestartGame(); // 게임을 재시작합니다.
		}
	}
}

// OnMatchStateSet 메서드. 매치 상태가 설정되면 호출됩니다.
void ALetsShootGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	// 월드 내의 모든 플레이어 컨트롤러를 반복하며
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ALetsShootPlayerController* LetsShootPlayer = Cast<ALetsShootPlayerController>(*It); // 현재의 PlayerController를 ALetsShootPlayerController로 캐스팅합니다.
		if (LetsShootPlayer) // 캐스팅이 성공했다면
		{
			LetsShootPlayer->OnMatchStateSet(MatchState, bTeamsMatch); // 해당 플레이어의 OnMatchStateSet 메서드를 호출합니다.
		}
	}
}

// CalculateDamage 메서드: 데미지를 계산하는 함수입니다.
// @param Attacker: 공격자의 컨트롤러
// @param Victim: 피해자의 컨트롤러
// @param BaseDamage: 기본 데미지 값
// 현재 버전에서는 데미지 계산이 없으므로 기본 데미지를 그대로 반환합니다.
float ALetsShootGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	return BaseDamage;
}

// PlayerEliminated 메서드: 플레이어가 죽었을 때 호출되는 함수입니다.
// @param ElimmedCharacter: 죽은 캐릭터
// @param VictimController: 죽은 플레이어의 컨트롤러
// @param AttackerController: 살인자의 컨트롤러
void ALetsShootGameMode::PlayerEliminated(class ALetsShootCharacter* ElimmedCharacter, class ALetsShootPlayerController* VictimController, ALetsShootPlayerController* AttackerController)
{
	// AttackerController 혹은 VictimController가 nullptr이거나 PlayerState가 nullptr인 경우 함수를 종료합니다.
	if (AttackerController == nullptr || AttackerController->PlayerState == nullptr) return;
	if (VictimController == nullptr || VictimController->PlayerState == nullptr) return;
	ALetsShootPlayerState* AttackerPlayerState = AttackerController ? Cast<ALetsShootPlayerState>(AttackerController->PlayerState) : nullptr;
	ALetsShootPlayerState* VictimPlayerState = VictimController ? Cast<ALetsShootPlayerState>(VictimController->PlayerState) : nullptr;

	// 게임 상태를 가져옵니다.
	ALetsShootGameState* LetsShootGameState = GetGameState<ALetsShootGameState>();

	// 공격자가 스스로를 죽이지 않았으며, 게임 상태가 존재하는 경우
	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState && LetsShootGameState)
	{
		// 현재 리드하는 플레이어 목록을 가져옵니다.
		TArray<ALetsShootPlayerState*> PlayersCurrentlyInTheLead;
		for (auto LeadPlayer : LetsShootGameState->TopScoringPlayers)
		{
			PlayersCurrentlyInTheLead.Add(LeadPlayer);
		}

		// 공격자의 점수를 증가시키고, 최고 점수를 업데이트합니다.
		AttackerPlayerState->AddToScore(1.f);
		LetsShootGameState->UpdateTopScore(AttackerPlayerState);

		// 공격자가 최고 점수를 기록한 플레이어이면
		if (LetsShootGameState->TopScoringPlayers.Contains(AttackerPlayerState))
		{
			ALetsShootCharacter* Leader = Cast<ALetsShootCharacter>(AttackerPlayerState->GetPawn());
			if (Leader)
			{
				Leader->MulticastGainedTheLead(); // 리더로서 리드를 얻었음을 알립니다.
			}
		}

		// 이전에 리더였지만 이제 리더가 아닌 플레이어에게 리드를 잃었다는 메시지를 전달합니다.
		for (int32 i = 0; i < PlayersCurrentlyInTheLead.Num(); i++)
		{
			if (!LetsShootGameState->TopScoringPlayers.Contains(PlayersCurrentlyInTheLead[i]))
			{
				ALetsShootCharacter* Loser = Cast<ALetsShootCharacter>(PlayersCurrentlyInTheLead[i]->GetPawn());
				if (Loser)
				{
					Loser->MulticastLostTheLead();
				}
			}
		}
	}

	// 피해자의 패배 횟수를 증가시킵니다.
	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}

	// ElimmedCharacter가 존재하면 이 캐릭터를 제거합니다.
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim(false);
	}

	// 모든 플레이어 컨트롤러에게 제거 이벤트를 알립니다.
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ALetsShootPlayerController* LetsShootPlayer = Cast<ALetsShootPlayerController>(*It);
		if (LetsShootPlayer && AttackerPlayerState && VictimPlayerState)
		{
			LetsShootPlayer->BroadcastElim(AttackerPlayerState, VictimPlayerState);
		}
	}
}

// RequestRespawn 메서드: 캐릭터의 재생성을 요청합니다.
// @param ElimmedCharacter: 죽은 캐릭터
// @param ElimmedController: 죽은 캐릭터의 컨트롤러
void ALetsShootGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	// ElimmedCharacter가 존재하면 이 캐릭터를 초기화하고 제거합니다.
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}

	// ElimmedController가 존재하면 새로운 시작 위치에서 플레이어를 재시작합니다.
	if (ElimmedController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
	}
}
// PlayerLeftGame 메서드: 플레이어가 게임을 떠났을 때 호출되는 함수입니다.
// @param PlayerLeaving: 게임을 떠나는 플레이어의 상태
void ALetsShootGameMode::PlayerLeftGame(ALetsShootPlayerState* PlayerLeaving)
{
	// PlayerLeaving이 nullptr인 경우 함수를 종료합니다.
	if (PlayerLeaving == nullptr) return;

	// 게임의 현재 상태를 가져옵니다.
	ALetsShootGameState* LetsShootGameState = GetGameState<ALetsShootGameState>();

	// 떠나는 플레이어가 최고 점수를 기록한 플레이어 목록에 있었다면 목록에서 제거합니다.
	if (LetsShootGameState && LetsShootGameState->TopScoringPlayers.Contains(PlayerLeaving))
	{
		LetsShootGameState->TopScoringPlayers.Remove(PlayerLeaving);
	}

	// 떠나는 플레이어의 캐릭터를 가져와 Elim 함수를 호출하여 제거합니다.
	ALetsShootCharacter* CharacterLeaving = Cast<ALetsShootCharacter>(PlayerLeaving->GetPawn());
	if (CharacterLeaving)
	{
		CharacterLeaving->Elim(true);
	}
}

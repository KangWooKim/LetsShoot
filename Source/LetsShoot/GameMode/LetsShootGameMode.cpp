// Fill out your copyright notice in the Description page of Project Settings.


#include "LetsShootGameMode.h"
#include "LetsShoot/Character/LetsShootCharacter.h"
#include "LetsShoot/PlayerController/LetsShootPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "LetsShoot/PlayerState/LetsShootPlayerState.h"
#include "LetsShoot/GameState/LetsShootGameState.h"

// �̸� ���ǵ� ��ġ ������ �̸��� �����մϴ�.
namespace MatchState
{
	const FName Cooldown = FName("Cooldown");
}

// ALetsShootGameMode�� ������
ALetsShootGameMode::ALetsShootGameMode()
{
	bDelayedStart = true; // ������ ���۵Ǳ� ���� �����̸� �����մϴ�.
}

// BeginPlay �޼���. ������ ���۵Ǹ� ȣ��˴ϴ�.
void ALetsShootGameMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartingTime = GetWorld()->GetTimeSeconds(); // ���� ���� �ð��� ����մϴ�.
}

// Tick �޼���. �����Ӹ��� ȣ��˴ϴ�.
// @param DeltaTime: ���� �����Ӱ��� �ð� ����
void ALetsShootGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MatchState == MatchState::WaitingToStart) // ��ġ�� �����ϱ⸦ ��ٸ��� ���̶��
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime; // ī��Ʈ�ٿ� �ð��� ������Ʈ�մϴ�.
		if (CountdownTime <= 0.f) // ī��Ʈ�ٿ��� ������
		{
			StartMatch(); // ��ġ�� �����մϴ�.
		}
	}
	else if (MatchState == MatchState::InProgress) // ��ġ�� ���� ���̶��
	{
		CountdownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime; // ī��Ʈ�ٿ� �ð��� ������Ʈ�մϴ�.
		if (CountdownTime <= 0.f) // ��ġ �ð��� ������
		{
			SetMatchState(MatchState::Cooldown); // ��ġ ���¸� Cooldown���� �����մϴ�.
		}
	}
	else if (MatchState == MatchState::Cooldown) // ��ٿ� ���̶��
	{
		CountdownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime; // ī��Ʈ�ٿ� �ð��� ������Ʈ�մϴ�.
		if (CountdownTime <= 0.f) // ��ٿ��� ������
		{
			RestartGame(); // ������ ������մϴ�.
		}
	}
}

// OnMatchStateSet �޼���. ��ġ ���°� �����Ǹ� ȣ��˴ϴ�.
void ALetsShootGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	// ���� ���� ��� �÷��̾� ��Ʈ�ѷ��� �ݺ��ϸ�
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ALetsShootPlayerController* LetsShootPlayer = Cast<ALetsShootPlayerController>(*It); // ������ PlayerController�� ALetsShootPlayerController�� ĳ�����մϴ�.
		if (LetsShootPlayer) // ĳ������ �����ߴٸ�
		{
			LetsShootPlayer->OnMatchStateSet(MatchState, bTeamsMatch); // �ش� �÷��̾��� OnMatchStateSet �޼��带 ȣ���մϴ�.
		}
	}
}

// CalculateDamage �޼���: �������� ����ϴ� �Լ��Դϴ�.
// @param Attacker: �������� ��Ʈ�ѷ�
// @param Victim: �������� ��Ʈ�ѷ�
// @param BaseDamage: �⺻ ������ ��
// ���� ���������� ������ ����� �����Ƿ� �⺻ �������� �״�� ��ȯ�մϴ�.
float ALetsShootGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	return BaseDamage;
}

// PlayerEliminated �޼���: �÷��̾ �׾��� �� ȣ��Ǵ� �Լ��Դϴ�.
// @param ElimmedCharacter: ���� ĳ����
// @param VictimController: ���� �÷��̾��� ��Ʈ�ѷ�
// @param AttackerController: �������� ��Ʈ�ѷ�
void ALetsShootGameMode::PlayerEliminated(class ALetsShootCharacter* ElimmedCharacter, class ALetsShootPlayerController* VictimController, ALetsShootPlayerController* AttackerController)
{
	// AttackerController Ȥ�� VictimController�� nullptr�̰ų� PlayerState�� nullptr�� ��� �Լ��� �����մϴ�.
	if (AttackerController == nullptr || AttackerController->PlayerState == nullptr) return;
	if (VictimController == nullptr || VictimController->PlayerState == nullptr) return;
	ALetsShootPlayerState* AttackerPlayerState = AttackerController ? Cast<ALetsShootPlayerState>(AttackerController->PlayerState) : nullptr;
	ALetsShootPlayerState* VictimPlayerState = VictimController ? Cast<ALetsShootPlayerState>(VictimController->PlayerState) : nullptr;

	// ���� ���¸� �����ɴϴ�.
	ALetsShootGameState* LetsShootGameState = GetGameState<ALetsShootGameState>();

	// �����ڰ� �����θ� ������ �ʾ�����, ���� ���°� �����ϴ� ���
	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState && LetsShootGameState)
	{
		// ���� �����ϴ� �÷��̾� ����� �����ɴϴ�.
		TArray<ALetsShootPlayerState*> PlayersCurrentlyInTheLead;
		for (auto LeadPlayer : LetsShootGameState->TopScoringPlayers)
		{
			PlayersCurrentlyInTheLead.Add(LeadPlayer);
		}

		// �������� ������ ������Ű��, �ְ� ������ ������Ʈ�մϴ�.
		AttackerPlayerState->AddToScore(1.f);
		LetsShootGameState->UpdateTopScore(AttackerPlayerState);

		// �����ڰ� �ְ� ������ ����� �÷��̾��̸�
		if (LetsShootGameState->TopScoringPlayers.Contains(AttackerPlayerState))
		{
			ALetsShootCharacter* Leader = Cast<ALetsShootCharacter>(AttackerPlayerState->GetPawn());
			if (Leader)
			{
				Leader->MulticastGainedTheLead(); // �����μ� ���带 ������� �˸��ϴ�.
			}
		}

		// ������ ���������� ���� ������ �ƴ� �÷��̾�� ���带 �Ҿ��ٴ� �޽����� �����մϴ�.
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

	// �������� �й� Ƚ���� ������ŵ�ϴ�.
	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}

	// ElimmedCharacter�� �����ϸ� �� ĳ���͸� �����մϴ�.
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim(false);
	}

	// ��� �÷��̾� ��Ʈ�ѷ����� ���� �̺�Ʈ�� �˸��ϴ�.
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ALetsShootPlayerController* LetsShootPlayer = Cast<ALetsShootPlayerController>(*It);
		if (LetsShootPlayer && AttackerPlayerState && VictimPlayerState)
		{
			LetsShootPlayer->BroadcastElim(AttackerPlayerState, VictimPlayerState);
		}
	}
}

// RequestRespawn �޼���: ĳ������ ������� ��û�մϴ�.
// @param ElimmedCharacter: ���� ĳ����
// @param ElimmedController: ���� ĳ������ ��Ʈ�ѷ�
void ALetsShootGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	// ElimmedCharacter�� �����ϸ� �� ĳ���͸� �ʱ�ȭ�ϰ� �����մϴ�.
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}

	// ElimmedController�� �����ϸ� ���ο� ���� ��ġ���� �÷��̾ ������մϴ�.
	if (ElimmedController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
	}
}
// PlayerLeftGame �޼���: �÷��̾ ������ ������ �� ȣ��Ǵ� �Լ��Դϴ�.
// @param PlayerLeaving: ������ ������ �÷��̾��� ����
void ALetsShootGameMode::PlayerLeftGame(ALetsShootPlayerState* PlayerLeaving)
{
	// PlayerLeaving�� nullptr�� ��� �Լ��� �����մϴ�.
	if (PlayerLeaving == nullptr) return;

	// ������ ���� ���¸� �����ɴϴ�.
	ALetsShootGameState* LetsShootGameState = GetGameState<ALetsShootGameState>();

	// ������ �÷��̾ �ְ� ������ ����� �÷��̾� ��Ͽ� �־��ٸ� ��Ͽ��� �����մϴ�.
	if (LetsShootGameState && LetsShootGameState->TopScoringPlayers.Contains(PlayerLeaving))
	{
		LetsShootGameState->TopScoringPlayers.Remove(PlayerLeaving);
	}

	// ������ �÷��̾��� ĳ���͸� ������ Elim �Լ��� ȣ���Ͽ� �����մϴ�.
	ALetsShootCharacter* CharacterLeaving = Cast<ALetsShootCharacter>(PlayerLeaving->GetPawn());
	if (CharacterLeaving)
	{
		CharacterLeaving->Elim(true);
	}
}
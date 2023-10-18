// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamFightGameMode.h"
#include "LetsShoot/GameState/LetsShootGameState.h"
#include "LetsShoot/PlayerState/LetsShootPlayerState.h"
#include "LetsShoot/PlayerController/LetsShootPlayerController.h"
#include "Kismet/GameplayStatics.h"

// ATeamFightGameMode�� ������
ATeamFightGameMode::ATeamFightGameMode()
{
	// �� ��ġ�� true�� �����մϴ�.
	bTeamsMatch = true;
}

// ���ο� �÷��̾ �α������� �� ȣ��Ǵ� �޼���
void ATeamFightGameMode::PostLogin(APlayerController* NewPlayer)
{
	// �θ� Ŭ������ PostLogin �޼��� ȣ��
	Super::PostLogin(NewPlayer);

	// ���� ���¸� ��������, ALetsShootGameState�� ĳ�����մϴ�.
	ALetsShootGameState* BGameState = Cast<ALetsShootGameState>(UGameplayStatics::GetGameState(this));
	if (BGameState)
	{
		// �÷��̾� ���¸� ��������, ALetsShootPlayerState�� ĳ�����մϴ�.
		ALetsShootPlayerState* BPState = NewPlayer->GetPlayerState<ALetsShootPlayerState>();

		// �÷��̾� ���°� ��ȿ�ϰ�, ���� �������� ���� ���
		if (BPState && BPState->GetTeam() == ETeam::ET_NoTeam)
		{
			// ������� �ο� ���� ���������� ������ �������� �߰�
			if (BGameState->BlueTeam.Num() >= BGameState->RedTeam.Num())
			{
				BGameState->RedTeam.AddUnique(BPState);
				BPState->SetTeam(ETeam::ET_RedTeam);
			}
			// �׷��� ������ ������� �߰�
			else
			{
				BGameState->BlueTeam.AddUnique(BPState);
				BPState->SetTeam(ETeam::ET_BlueTeam);
			}
		}
	}
}

// �÷��̾ �α׾ƿ��� �� ȣ��Ǵ� �޼���
void ATeamFightGameMode::Logout(AController* Exiting)
{
	// ���� ���¸� ��������, ALetsShootGameState�� ĳ�����մϴ�.
	ALetsShootGameState* BGameState = Cast<ALetsShootGameState>(UGameplayStatics::GetGameState(this));

	// �÷��̾� ���¸� ��������, ALetsShootPlayerState�� ĳ�����մϴ�.
	ALetsShootPlayerState* BPState = Exiting->GetPlayerState<ALetsShootPlayerState>();
	if (BGameState && BPState)
	{
		// �������� �ִ� ���, ���������� ����
		if (BGameState->RedTeam.Contains(BPState))
		{
			BGameState->RedTeam.Remove(BPState);
		}
		// ������� �ִ� ���, ��������� ����
		if (BGameState->BlueTeam.Contains(BPState))
		{
			BGameState->BlueTeam.Remove(BPState);
		}
	}
}


// ��ġ�� ���۵Ǿ��� �� ȣ��Ǵ� �޼���
void ATeamFightGameMode::HandleMatchHasStarted()
{
	// �θ� Ŭ������ HandleMatchHasStarted �޼��� ȣ��
	Super::HandleMatchHasStarted();

	// ���� ���¸� ��������, ALetsShootGameState�� ĳ�����մϴ�.
	ALetsShootGameState* BGameState = Cast<ALetsShootGameState>(UGameplayStatics::GetGameState(this));
	if (BGameState)
	{
		// �÷��̾� �迭�� ��ȸ�մϴ�.
		for (auto PState : BGameState->PlayerArray)
		{
			// �÷��̾� ���¸� ��������, ALetsShootPlayerState�� ĳ�����մϴ�.
			ALetsShootPlayerState* BPState = Cast<ALetsShootPlayerState>(PState.Get());

			// �÷��̾� ���°� ��ȿ�ϰ�, ���� �������� ���� ���
			if (BPState && BPState->GetTeam() == ETeam::ET_NoTeam)
			{
				// ������� �ο� ���� ���������� ������ �������� �߰�
				if (BGameState->BlueTeam.Num() >= BGameState->RedTeam.Num())
				{
					BGameState->RedTeam.AddUnique(BPState);
					BPState->SetTeam(ETeam::ET_RedTeam);
				}
				// �׷��� ������ ������� �߰�
				else
				{
					BGameState->BlueTeam.AddUnique(BPState);
					BPState->SetTeam(ETeam::ET_BlueTeam);
				}
			}
		}
	}
}

// �����ڰ� �����ڿ��� ���� �������� ����ϴ� �޼���
// Attacker : �������� ��Ʈ�ѷ�
// Victim : �������� ��Ʈ�ѷ�
// BaseDamage : �⺻ ������ ��
float ATeamFightGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	// �����ڿ� �������� �÷��̾� ���¸� �����ɴϴ�.
	ALetsShootPlayerState* AttackerPState = Attacker->GetPlayerState<ALetsShootPlayerState>();
	ALetsShootPlayerState* VictimPState = Victim->GetPlayerState<ALetsShootPlayerState>();

	// �����ڳ� �������� ���°� ��ȿ���� �ʴٸ�, �⺻ �������� ��ȯ�մϴ�.
	if (AttackerPState == nullptr || VictimPState == nullptr) return BaseDamage;

	// �ڱ� �ڽ��� �����ϴ� ���, �⺻ �������� ��ȯ�մϴ�.
	if (VictimPState == AttackerPState)
	{
		return BaseDamage;
	}

	// ���� ���� �÷��̾ �����ϴ� ���, �������� 0�Դϴ�.
	if (AttackerPState->GetTeam() == VictimPState->GetTeam())
	{
		return 0.f;
	}

	// �׷��� ���� ���, �⺻ �������� ��ȯ�մϴ�.
	return BaseDamage;
}


// �÷��̾ ���ŵ��� �� ȣ��Ǵ� �޼���
// ElimmedCharacter : ���ŵ� ĳ����
// VictimController : �������� �÷��̾� ��Ʈ�ѷ�
// AttackerController : �������� �÷��̾� ��Ʈ�ѷ�
void ATeamFightGameMode::PlayerEliminated(ALetsShootCharacter* ElimmedCharacter, ALetsShootPlayerController* VictimController, ALetsShootPlayerController* AttackerController)
{
	// �θ� Ŭ������ PlayerEliminated �޼��� ȣ��
	Super::PlayerEliminated(ElimmedCharacter, VictimController, AttackerController);

	// ���� ���¸� ��������, ALetsShootGameState�� ĳ�����մϴ�.
	ALetsShootGameState* BGameState = Cast<ALetsShootGameState>(UGameplayStatics::GetGameState(this));

	// �������� �÷��̾� ���¸� �����ɴϴ�.
	ALetsShootPlayerState* AttackerPlayerState = AttackerController ? Cast<ALetsShootPlayerState>(AttackerController->PlayerState) : nullptr;

	// ���� ���¿� �������� �÷��̾� ���°� ��ȿ�� ���
	if (BGameState && AttackerPlayerState)
	{
		// �����ڰ� ������̶��, ������� ������ �ø��ϴ�.
		if (AttackerPlayerState->GetTeam() == ETeam::ET_BlueTeam)
		{
			BGameState->BlueTeamScores();
		}
		// �����ڰ� �������̶��, �������� ������ �ø��ϴ�.
		if (AttackerPlayerState->GetTeam() == ETeam::ET_RedTeam)
		{
			BGameState->RedTeamScores();
		}
	}
}

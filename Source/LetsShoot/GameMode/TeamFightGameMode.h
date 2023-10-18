// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LetsShootGameMode.h"
#include "TeamFightGameMode.generated.h"

/**
 * 
 */
UCLASS()
// �� ���� ��带 ��Ÿ���� Ŭ����
class LETSSHOOT_API ATeamFightGameMode : public ALetsShootGameMode
{
	GENERATED_BODY()
public:
	// ������
	ATeamFightGameMode();

	// ���ο� �÷��̾ �α������� �� ȣ��Ǵ� �޼���
	// NewPlayer : �α����� �÷��̾� ��Ʈ�ѷ�
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// �÷��̾ �α׾ƿ����� �� ȣ��Ǵ� �޼���
	// Exiting : �α׾ƿ��ϴ� ��Ʈ�ѷ�
	virtual void Logout(AController* Exiting) override;

	// �������� ����ϴ� �޼���
	// Attacker : ������ ��Ʈ�ѷ�
	// Victim : ������ ��Ʈ�ѷ�
	// BaseDamage : �⺻ ������ ��
	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage) override;

	// �÷��̾ ���ŵǾ��� �� ȣ��Ǵ� �޼���
	// ElimmedCharacter : ���ŵ� ĳ����
	// VictimController : �������� �÷��̾� ��Ʈ�ѷ�
	// AttackerController : �������� �÷��̾� ��Ʈ�ѷ�
	virtual void PlayerEliminated(class ALetsShootCharacter* ElimmedCharacter, class ALetsShootPlayerController* VictimController, ALetsShootPlayerController* AttackerController) override;

protected:
	// ��Ⱑ ���۵Ǿ��� �� ȣ��Ǵ� �޼���
	virtual void HandleMatchHasStarted() override;
};


// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "LetsShoot/LetsShootTypes/Team.h"
#include "LetsShootPlayerState.generated.h"

/**
 * 
 */
 // �÷��̾��� ���¸� ��Ÿ���� Ŭ����
UCLASS()
class LETSSHOOT_API ALetsShootPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	// ������Ÿ�� ���� �����ؾ��ϴ� �Ӽ��� �����ɴϴ�. 
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	/**
	* ���� �˸�
	*/
	// ������ ������ �� ȣ��Ǵ� �Լ�
	virtual void OnRep_Score() override;

	// �й谡 ������ �� ȣ��Ǵ� �Լ�
	UFUNCTION()
		virtual void OnRep_Defeats();

	// ������ �߰��ϴ� �Լ�, ScoreAmount - �߰��� ������ ��
	void AddToScore(float ScoreAmount);

	// �й踦 �߰��ϴ� �Լ�, DefeatsAmount - �߰��� �й��� ��
	void AddToDefeats(int32 DefeatsAmount);

private:
	// �÷��̾� ĳ������ ����
	UPROPERTY()
		class ALetsShootCharacter* Character;

	// �÷��̾� ��Ʈ�ѷ��� ����
	UPROPERTY()
		class ALetsShootPlayerController* Controller;

	// �й��� ��, ���� �� OnRep_Defeats�� ȣ���
	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
		int32 Defeats;

	// �÷��̾��� ��, ���� �� OnRep_Team�� ȣ���
	UPROPERTY(ReplicatedUsing = OnRep_Team)
		ETeamColor Team = ETeam::ET_NoTeam;

	// ���� ������ �� ȣ��Ǵ� �Լ�
	UFUNCTION()
		void OnRep_Team();

public:
	// ���� �������� �ζ��� �Լ�, ��ȯ��: ���� ��
	FORCEINLINE ETeamColor GetTeam() const { return Team; }

	// ���� �����ϴ� �Լ�, TeamToSet - ������ ��
	void SetTeam(ETeamColor TeamToSet);
};

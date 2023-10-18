// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TeamsGameMode.h"
#include "GetFlagGameMode.generated.h"

/**
 * 
 */
UCLASS()
// Capture The Flag ���� ��� Ŭ����
class LETSSHOOT_API AGetFlagGameMode : public ATeamsGameMode
{
	GENERATED_BODY()

public:
	// �÷��̾ ���ӿ��� ���ܵ� �� ȣ��Ǵ� �޼���
	// ElimmedCharacter: ���ӿ��� ���ܵ� ĳ���Ϳ� ���� ����
	// VictimController: ����� �÷��̾��� ��Ʈ�ѷ��� ���� ����
	// AttackerController: ������ �÷��̾��� ��Ʈ�ѷ��� ���� ����
	virtual void PlayerEliminated(class ALetsShootCharacter* ElimmedCharacter, class ALetsShootPlayerController* VictimController, ALetsShootPlayerController* AttackerController) override;

	// ����� ĸó�� �� ȣ��Ǵ� �޼���
	// Flag: ĸó�� ��߿� ���� ����
	// Zone: ����� ĸó�� ���� ���� ����
	void FlagCaptured(class AFlag* Flag, class AFlagZone* Zone);
};


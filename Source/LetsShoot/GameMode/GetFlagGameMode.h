// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TeamsGameMode.h"
#include "GetFlagGameMode.generated.h"

/**
 * 
 */
UCLASS()
// Capture The Flag 게임 모드 클래스
class LETSSHOOT_API AGetFlagGameMode : public ATeamsGameMode
{
	GENERATED_BODY()

public:
	// 플레이어가 게임에서 제외될 때 호출되는 메서드
	// ElimmedCharacter: 게임에서 제외된 캐릭터에 대한 참조
	// VictimController: 희생된 플레이어의 컨트롤러에 대한 참조
	// AttackerController: 공격한 플레이어의 컨트롤러에 대한 참조
	virtual void PlayerEliminated(class ALetsShootCharacter* ElimmedCharacter, class ALetsShootPlayerController* VictimController, ALetsShootPlayerController* AttackerController) override;

	// 깃발이 캡처될 때 호출되는 메서드
	// Flag: 캡처된 깃발에 대한 참조
	// Zone: 깃발이 캡처된 존에 대한 참조
	void FlagCaptured(class AFlag* Flag, class AFlagZone* Zone);
};


// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LetsShootGameMode.generated.h"

namespace MatchState
{
	extern LETSSHOOT_API const FName Cooldown; // 매치 시간이 종료되었습니다. 승자를 표시하고 쿨다운 타이머를 시작합니다.
}

/**
 *
 */
UCLASS()
class LETSSHOOT_API ALetsShootGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	ALetsShootGameMode();  // 생성자
	virtual void Tick(float DeltaTime) override;  // Tick 메서드는 게임 프레임마다 호출됩니다. DeltaTime은 이전 프레임에서 현재 프레임까지의 시간 간격입니다.
	virtual void PlayerEliminated(class ALetsShootCharacter* ElimmedCharacter, class ALetsShootPlayerController* VictimController, ALetsShootPlayerController* AttackerController);  // 플레이어가 제거될 때 호출됩니다.
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);  // 캐릭터의 부활을 요청합니다.
	void PlayerLeftGame(class ALetsShootPlayerState* PlayerLeaving);  // 플레이어가 게임을 나갔을 때 호출됩니다.
	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage);  // 데미지 계산 메서드입니다. 

	// 게임에서 사용되는 변수들입니다.
	UPROPERTY(EditDefaultsOnly)
		float WarmupTime = 10.f;  // 게임 시작 전 웜업 시간입니다.

	UPROPERTY(EditDefaultsOnly)
		float MatchTime = 120.f;  // 한 게임의 총 시간입니다.

	UPROPERTY(EditDefaultsOnly)
		float CooldownTime = 10.f;  // 쿨다운 시간입니다.

	float LevelStartingTime = 0.f;  // 레벨이 시작된 시간입니다.

	bool bTeamsMatch = false;  // 팀 매치 여부를 나타내는 플래그입니다.
protected:
	virtual void BeginPlay() override;  // 플레이 시작 시 호출됩니다.
	virtual void OnMatchStateSet() override;  // 매치 상태가 설정될 때 호출됩니다.

private:
	float CountdownTime = 0.f;  // 카운트다운 시간입니다.
public:
	FORCEINLINE float GetCountdownTime() const { return CountdownTime; }  // 카운트다운 시간을 가져오는 메서드입니다.
};

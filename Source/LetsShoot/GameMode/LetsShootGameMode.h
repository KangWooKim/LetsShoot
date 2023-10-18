// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LetsShootGameMode.generated.h"

namespace MatchState
{
	extern LETSSHOOT_API const FName Cooldown; // ��ġ �ð��� ����Ǿ����ϴ�. ���ڸ� ǥ���ϰ� ��ٿ� Ÿ�̸Ӹ� �����մϴ�.
}

/**
 *
 */
UCLASS()
class LETSSHOOT_API ALetsShootGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	ALetsShootGameMode();  // ������
	virtual void Tick(float DeltaTime) override;  // Tick �޼���� ���� �����Ӹ��� ȣ��˴ϴ�. DeltaTime�� ���� �����ӿ��� ���� �����ӱ����� �ð� �����Դϴ�.
	virtual void PlayerEliminated(class ALetsShootCharacter* ElimmedCharacter, class ALetsShootPlayerController* VictimController, ALetsShootPlayerController* AttackerController);  // �÷��̾ ���ŵ� �� ȣ��˴ϴ�.
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);  // ĳ������ ��Ȱ�� ��û�մϴ�.
	void PlayerLeftGame(class ALetsShootPlayerState* PlayerLeaving);  // �÷��̾ ������ ������ �� ȣ��˴ϴ�.
	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage);  // ������ ��� �޼����Դϴ�. 

	// ���ӿ��� ���Ǵ� �������Դϴ�.
	UPROPERTY(EditDefaultsOnly)
		float WarmupTime = 10.f;  // ���� ���� �� ���� �ð��Դϴ�.

	UPROPERTY(EditDefaultsOnly)
		float MatchTime = 120.f;  // �� ������ �� �ð��Դϴ�.

	UPROPERTY(EditDefaultsOnly)
		float CooldownTime = 10.f;  // ��ٿ� �ð��Դϴ�.

	float LevelStartingTime = 0.f;  // ������ ���۵� �ð��Դϴ�.

	bool bTeamsMatch = false;  // �� ��ġ ���θ� ��Ÿ���� �÷����Դϴ�.
protected:
	virtual void BeginPlay() override;  // �÷��� ���� �� ȣ��˴ϴ�.
	virtual void OnMatchStateSet() override;  // ��ġ ���°� ������ �� ȣ��˴ϴ�.

private:
	float CountdownTime = 0.f;  // ī��Ʈ�ٿ� �ð��Դϴ�.
public:
	FORCEINLINE float GetCountdownTime() const { return CountdownTime; }  // ī��Ʈ�ٿ� �ð��� �������� �޼����Դϴ�.
};
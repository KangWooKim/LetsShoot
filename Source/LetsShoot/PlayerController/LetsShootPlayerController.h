// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LetsShootPlayerController.generated.h"

// 높은 핑 상태를 위한 델리게이트 선언입니다.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighPingDelegate, bool, bPingTooHigh);

// LetsShoot 플레이어 컨트롤러 클래스 선언입니다.
UCLASS()
class LETSSHOOT_API ALetsShootPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// HUD에 플레이어의 체력을 설정하는 메서드입니다. Health: 현재 체력, MaxHealth: 최대 체력
	void SetHUDHealth(float Health, float MaxHealth);

	// HUD에 플레이어의 방어막을 설정하는 메서드입니다. Shield: 현재 방어막, MaxShield: 최대 방어막
	void SetHUDShield(float Shield, float MaxShield);

	// HUD에 플레이어의 점수를 설정하는 메서드입니다. Score: 현재 점수
	void SetHUDScore(float Score);

	// HUD에 플레이어의 패배 횟수를 설정하는 메서드입니다. Defeats: 패배 횟수
	void SetHUDDefeats(int32 Defeats);

	// HUD에 플레이어의 현재 무기 탄약을 설정하는 메서드입니다. Ammo: 현재 탄약 수
	void SetHUDWeaponAmmo(int32 Ammo);

	// HUD에 플레이어가 소지한 탄약을 설정하는 메서드입니다. Ammo: 소지한 탄약 수
	void SetHUDCarriedAmmo(int32 Ammo);

	// HUD에 매치 카운트다운을 설정하는 메서드입니다. CountdownTime: 남은 시간
	void SetHUDMatchCountdown(float CountdownTime);

	// HUD에 알림 카운트다운을 설정하는 메서드입니다. CountdownTime: 남은 시간
	void SetHUDAnnouncementCountdown(float CountdownTime);

	// HUD에 수류탄 수를 설정하는 메서드입니다. Grenades: 수류탄 수
	void SetHUDGrenades(int32 Grenades);

	// 캐릭터를 점령하면 호출되는 메서드입니다. InPawn: 점령한 캐릭터
	virtual void OnPossess(APawn* InPawn) override;

	// 매 틱마다 호출되는 메서드입니다. DeltaTime: 이전 틱 이후의 시간
	virtual void Tick(float DeltaTime) override;

	// 생애 동안 복제되는 속성들을 가져옵니다. OutLifetimeProps: 생애 동안 복제되는 속성들
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 팀 점수를 숨기는 메서드입니다.
	void HideTeamScores();

	// 팀 점수를 초기화하는 메서드입니다.
	void InitTeamScores();

	// HUD에 레드 팀의 점수를 설정하는 메서드입니다. RedScore: 레드 팀 점수
	void SetHUDRedTeamScore(int32 RedScore);

	// HUD에 블루 팀의 점수를 설정하는 메서드입니다. BlueScore: 블루 팀 점수
	void SetHUDBlueTeamScore(int32 BlueScore);

	// 서버 시간을 동기화해서 가져오는 메서드입니다.
	virtual float GetServerTime();

	// 가능한 빨리 서버 클럭과 동기화하는 메서드입니다.
	virtual void ReceivedPlayer() override;

	// 매치 상태가 설정되면 호출되는 메서드입니다. State: 매치 상태, bTeamsMatch: 팀 매치인지 아닌지
	void OnMatchStateSet(FName State, bool bTeamsMatch = false);

	// 매치가 시작되면 호출되는 메서드입니다. bTeamsMatch: 팀 매치인지 아닌지
	void HandleMatchHasStarted(bool bTeamsMatch = false);

	// 쿨다운 처리하는 메서드입니다.
	void HandleCooldown();

	// 단일 여행 시간입니다.
	float SingleTripTime = 0.f;

	// 높은 핑 델리게이트입니다.
	FHighPingDelegate HighPingDelegate;

	// 체결 이벤트를 방송하는 메서드입니다. Attacker: 공격자, Victim: 피해자
	void BroadcastElim(APlayerState* Attacker, APlayerState* Victim);
protected:
    // 게임이 시작될 때 호출되는 메서드입니다.
    virtual void BeginPlay() override;

    // HUD에 시간을 설정하는 메서드입니다.
    void SetHUDTime();

    // 초기화 폴링을 수행하는 메서드입니다.
    void PollInit();

    // 입력 컴포넌트를 설정하는 메서드입니다.
    virtual void SetupInputComponent() override;

    // 클라이언트와 서버 간의 시간을 동기화하는 메서드입니다.
    // 현재 서버 시간을 요청하고, 클라이언트의 요청 시간을 전달합니다.
	// @param TimeOfClientRequest: 클라이언트가 요청을 보냈을 때의 클라이언트의 시간입니다.
    UFUNCTION(Server, Reliable)
    void ServerRequestServerTime(float TimeOfClientRequest);

    // ServerRequestServerTime에 대한 응답으로 현재 서버 시간을 클라이언트에게 보고하는 메서드입니다.
	// @param TimeOfClientRequest: 클라이언트가 요청을 보냈을 때의 클라이언트의 시간입니다.
	// @param TimeServerReceivedClientRequest: 클라이언트 요청을 서버가 받았을 때의 서버 시간입니다.
    UFUNCTION(Client, Reliable)
    void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

    // 클라이언트와 서버 시간의 차이입니다.
    float ClientServerDelta = 0.f;

    // 시간 동기화 주기입니다.
    UPROPERTY(EditAnywhere, Category = Time)
    float TimeSyncFrequency = 5.f;

    // 시간 동기화를 위한 러닝 시간입니다.
    float TimeSyncRunningTime = 0.f;

    // 시간 동기화를 확인하는 메서드입니다.
	// @param DeltaTime: 이전 프레임 이후의 경과 시간입니다.
    void CheckTimeSync(float DeltaTime);

    // 매치 상태를 확인하는 메서드입니다.
    UFUNCTION(Server, Reliable)
    void ServerCheckMatchState();

    // 게임 중간에 참가할 때 호출되는 메서드입니다.
	// @param StateOfMatch: 현재 매치의 상태입니다.
	// @param Warmup: 웜업 시간입니다.
	// @param Match: 매치 시간입니다.
	// @param Cooldown: 쿨다운 시간입니다.
	// @param StartingTime: 시작 시간입니다.
    UFUNCTION(Client, Reliable)
    void ClientJoinMidgame(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime);

    // 높은 핑 경고를 보여주는 메서드입니다.
    void HighPingWarning();

    // 높은 핑 경고를 중지하는 메서드입니다.
    void StopHighPingWarning();

    // 핑을 확인하는 메서드입니다.
	// @param DeltaTime: 이전 프레임 이후의 경과 시간입니다.
    void CheckPing(float DeltaTime);

    // 메인 메뉴로 돌아가기를 보여주는 메서드입니다.
    void ShowReturnToMainMenu();

    // 체결 알림을 클라이언트에게 전송하는 메서드입니다.
	// @param Attacker: 공격자의 플레이어 상태입니다.
	// @param Victim: 피해자의 플레이어 상태입니다.
    UFUNCTION(Client, Reliable)
    void ClientElimAnnouncement(APlayerState* Attacker, APlayerState* Victim);

    // 팀 점수를 보여줄지 여부입니다.
    UPROPERTY(ReplicatedUsing = OnRep_ShowTeamScores)
    bool bShowTeamScores = false;

    // 팀 점수 표시 변경 시 호출되는 메서드입니다.
    UFUNCTION()
    void OnRep_ShowTeamScores();

    // 플레이어들에 대한 정보 텍스트를 가져오는 메서드입니다.
	// @param Players: 정보를 얻고자 하는 플레이어들의 배열입니다.
    FString GetInfoText(const TArray<class ALetsShootPlayerState*>& Players);

    // 팀 정보 텍스트를 가져오는 메서드입니다.
	// @param LetsShootGameState: 현재 게임 상태입니다.
    FString GetTeamsInfoText(class ALetsShootGameState* LetsShootGameState);

private:
    // 현재 플레이어의 HUD를 가리킵니다.
    UPROPERTY()
        class ALetsShootHUD* LetsShootHUD;

    // 현재 게임 모드를 가리킵니다.
    UPROPERTY()
        class ALetsShootGameMode* LetsShootGameMode;

    // 메인 메뉴로 돌아가는 위젯을 설정합니다.
    UPROPERTY(EditAnywhere, Category = HUD)
        TSubclassOf<class UUserWidget> ReturnToMainMenuWidget;

    // 메인 메뉴로 돌아가기 위한 클래스를 가리킵니다.
    UPROPERTY()
        class UReturnToMainMenu* ReturnToMainMenu;

    // 메인 메뉴로 돌아가는지의 상태를 나타냅니다.
    bool bReturnToMainMenuOpen = false;

    // 게임의 시작 시간, 매치 시간, 웜업 시간, 쿨다운 시간을 정의합니다.
    float LevelStartingTime = 0.f;
    float MatchTime = 0.f;
    float WarmupTime = 0.f;
    float CooldownTime = 0.f;
    uint32 CountdownInt = 0;

    // 매치의 상태를 나타냅니다.
    UPROPERTY(ReplicatedUsing = OnRep_MatchState)
        FName MatchState;

    // 상태가 변경될 때 실행될 함수입니다.
    UFUNCTION()
        void OnRep_MatchState();

    // 캐릭터 오버레이를 가리킵니다.
    UPROPERTY()
        class UCharacterOverlay* CharacterOverlay;

    // HUD상의 체력, 점수, 패배, 수류탄, 쉴드, 탄약 등을 나타내는 변수들입니다.
    float HUDHealth;
    bool bInitializeHealth = false;
    float HUDMaxHealth;
    float HUDScore;
    bool bInitializeScore = false;
    int32 HUDDefeats;
    bool bInitializeDefeats = false;
    int32 HUDGrenades;
    bool bInitializeGrenades = false;
    float HUDShield;
    bool bInitializeShield = false;
    float HUDMaxShield;
    float HUDCarriedAmmo;
    bool bInitializeCarriedAmmo = false;
    float HUDWeaponAmmo;
    bool bInitializeWeaponAmmo = false;

    // 높은 핑을 감지하기 위한 변수들입니다.
    float HighPingRunningTime = 0.f;
    UPROPERTY(EditAnywhere)
        float HighPingDuration = 5.f;
    float PingAnimationRunningTime = 0.f;
    UPROPERTY(EditAnywhere)
        float CheckPingFrequency = 20.f;

    // 서버에 높은 핑 상태를 보고하는 함수입니다.
    UFUNCTION(Server, Reliable)
        void ServerReportPingStatus(bool bHighPing);

    // 높은 핑을 감지하는 임계값입니다.
    UPROPERTY(EditAnywhere)
        float HighPingThreshold = 50.f;

};

// Fill out your copyright notice in the Description page of Project Settings.


#include "LetsShootPlayerController.h"
#include "LetsShoot/HUD/LetsShootHUD.h"
#include "LetsShoot/HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "LetsShoot/Character/LetsShootCharacter.h"
#include "Net/UnrealNetwork.h"
#include "LetsShoot/GameMode/LetsShootGameMode.h"
#include "LetsShoot/PlayerState/LetsShootPlayerState.h"
#include "LetsShoot/HUD/Announcement.h"
#include "Kismet/GameplayStatics.h"
#include "LetsShoot/LetsShootComponents/CombatComponent.h"
#include "LetsShoot/GameState/LetsShootGameState.h"
#include "Components/Image.h"
#include "LetsShoot/HUD/BackToMenu.h"
#include "LetsShoot/LetsShootTypes/Announcement.h"

// �÷��̾� ��Ʈ�ѷ��� ���� Ŭ������ �����մϴ�.
void ALetsShootPlayerController::BroadcastElim(APlayerState* Attacker, APlayerState* Victim)
{
	// Ŭ���̾�Ʈ�κ��� �����ڿ� �����ڿ� ���� ������ �޾� ó���մϴ�.
	ClientElimAnnouncement(Attacker, Victim);
}

// Ŭ���̾�Ʈ�κ��� Elim(Ż��) �˸��� �����մϴ�.
void ALetsShootPlayerController::ClientElimAnnouncement_Implementation(APlayerState* Attacker, APlayerState* Victim)
{
	// ���� �÷��̾��� ���¸� �����ɴϴ�.
	APlayerState* Self = GetPlayerState<APlayerState>();

	// ������, ������, �׸��� ���� �÷��̾ ��� ������ ���
	if (Attacker && Victim && Self)
	{
		// LetsShootHUD�� �������ų� ���� HUD�� LetsShootHUD�� ĳ�����մϴ�.
		LetsShootHUD = LetsShootHUD == nullptr ? Cast<ALetsShootHUD>(GetHUD()) : LetsShootHUD;

		// LetsShootHUD�� ������ ���
		if (LetsShootHUD)
		{
			// ���� �÷��̾ �������̸�, �����ڴ� �ƴ� ���
			if (Attacker == Self && Victim != Self)
			{
				// �����ڿ� �������� �̸��� Elim �˸����� �߰��մϴ�.
				LetsShootHUD->AddElimAnnouncement("You", Victim->GetPlayerName());
				return;
			}
			// ���� �÷��̾ �������̸�, �����ڴ� �ƴ� ���
			if (Victim == Self && Attacker != Self)
			{
				// �����ڿ� �������� �̸��� Elim �˸����� �߰��մϴ�.
				LetsShootHUD->AddElimAnnouncement(Attacker->GetPlayerName(), "you");
				return;
			}
			// �����ڿ� �����ڰ� �����ϰ�, �װ��� ���� �÷��̾��� ���
			if (Attacker == Victim && Attacker == Self)
			{
				// �����ڿ� �������� �̸��� Elim �˸����� �߰��մϴ�.
				LetsShootHUD->AddElimAnnouncement("You", "yourself");
				return;
			}
			// �����ڿ� �����ڰ� �����ϳ�, �װ��� ���� �÷��̾�� �ƴ� ���
			if (Attacker == Victim && Attacker != Self)
			{
				// �����ڿ� �������� �̸��� Elim �˸����� �߰��մϴ�.
				LetsShootHUD->AddElimAnnouncement(Attacker->GetPlayerName(), "themselves");
				return;
			}

			// �� ���� ���, �����ڿ� �������� �̸��� Elim �˸����� �߰��մϴ�.
			LetsShootHUD->AddElimAnnouncement(Attacker->GetPlayerName(), Victim->GetPlayerName());
		}
	}
}

// �÷��� ���� �� ����Ǵ� �Լ��Դϴ�.
void ALetsShootPlayerController::BeginPlay()
{
	// �θ� Ŭ������ BeginPlay �Լ��� �����մϴ�.
	Super::BeginPlay();

	// ���� HUD�� LetsShootHUD�� ĳ�����մϴ�.
	LetsShootHUD = Cast<ALetsShootHUD>(GetHUD());

	// ������ ��ġ ���¸� Ȯ���մϴ�.
	ServerCheckMatchState();
}

// ���ø����̼ǿ� �ʿ��� �Ӽ����� �����մϴ�.
void ALetsShootPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	// ���� Ŭ������ ���� �Լ��� ȣ���մϴ�.
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// MatchState�� bShowTeamScores ������ ���ø����̼ǿ� ���Խ�ŵ�ϴ�.
	DOREPLIFETIME(ALetsShootPlayerController, MatchState);
	DOREPLIFETIME(ALetsShootPlayerController, bShowTeamScores);
}

// �� ������ ����� �޼����Դϴ�.
void ALetsShootPlayerController::HideTeamScores()
{
	// LetsShootHUD�� �������ų� ���� HUD�� LetsShootHUD�� ĳ�����մϴ�.
	LetsShootHUD = LetsShootHUD == nullptr ? Cast<ALetsShootHUD>(GetHUD()) : LetsShootHUD;

	// HUD�� ��ȿ���� �˻��մϴ�.
	bool bHUDValid = LetsShootHUD &&
		LetsShootHUD->CharacterOverlay &&
		LetsShootHUD->CharacterOverlay->RedTeamScore &&
		LetsShootHUD->CharacterOverlay->BlueTeamScore &&
		LetsShootHUD->CharacterOverlay->ScoreSpacerText;

	// HUD�� ��ȿ�� ���, �� ������ �ʱ�ȭ�մϴ�.
	if (bHUDValid)
	{
		LetsShootHUD->CharacterOverlay->RedTeamScore->SetText(FText());
		LetsShootHUD->CharacterOverlay->BlueTeamScore->SetText(FText());
		LetsShootHUD->CharacterOverlay->ScoreSpacerText->SetText(FText());
	}
}

// �� ������ �ʱ�ȭ�ϴ� �޼����Դϴ�.
void ALetsShootPlayerController::InitTeamScores()
{
	// LetsShootHUD�� �������ų� ���� HUD�� LetsShootHUD�� ĳ�����մϴ�.
	LetsShootHUD = LetsShootHUD == nullptr ? Cast<ALetsShootHUD>(GetHUD()) : LetsShootHUD;

	// HUD�� ��ȿ���� �˻��մϴ�.
	bool bHUDValid = LetsShootHUD &&
		LetsShootHUD->CharacterOverlay &&
		LetsShootHUD->CharacterOverlay->RedTeamScore &&
		LetsShootHUD->CharacterOverlay->BlueTeamScore &&
		LetsShootHUD->CharacterOverlay->ScoreSpacerText;

	// HUD�� ��ȿ�� ���, �� ������ �ʱ�ȭ�մϴ�.
	if (bHUDValid)
	{
		FString Zero("0");
		FString Spacer("|");
		LetsShootHUD->CharacterOverlay->RedTeamScore->SetText(FText::FromString(Zero));
		LetsShootHUD->CharacterOverlay->BlueTeamScore->SetText(FText::FromString(Zero));
		LetsShootHUD->CharacterOverlay->ScoreSpacerText->SetText(FText::FromString(Spacer));
	}
}


// HUD�� ������ ������ �����ϴ� �޼����Դϴ�.
void ALetsShootPlayerController::SetHUDRedTeamScore(int32 RedScore)
{
	// LetsShootHUD�� �������ų� ���� HUD�� LetsShootHUD�� ĳ�����մϴ�.
	LetsShootHUD = LetsShootHUD == nullptr ? Cast<ALetsShootHUD>(GetHUD()) : LetsShootHUD;

	// HUD�� ��ȿ���� �˻��մϴ�.
	bool bHUDValid = LetsShootHUD &&
		LetsShootHUD->CharacterOverlay &&
		LetsShootHUD->CharacterOverlay->RedTeamScore;

	// HUD�� ��ȿ�� ���, ������ ������ �����մϴ�.
	if (bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), RedScore);
		LetsShootHUD->CharacterOverlay->RedTeamScore->SetText(FText::FromString(ScoreText));
	}
}

// HUD�� �Ķ��� ������ �����ϴ� �޼����Դϴ�.
void ALetsShootPlayerController::SetHUDBlueTeamScore(int32 BlueScore)
{
	// LetsShootHUD�� �������ų� ���� HUD�� LetsShootHUD�� ĳ�����մϴ�.
	LetsShootHUD = LetsShootHUD == nullptr ? Cast<ALetsShootHUD>(GetHUD()) : LetsShootHUD;

	// HUD�� ��ȿ���� �˻��մϴ�.
	bool bHUDValid = LetsShootHUD &&
		LetsShootHUD->CharacterOverlay &&
		LetsShootHUD->CharacterOverlay->BlueTeamScore;

	// HUD�� ��ȿ�� ���, �Ķ��� ������ �����մϴ�.
	if (bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), BlueScore);
		LetsShootHUD->CharacterOverlay->BlueTeamScore->SetText(FText::FromString(ScoreText));
	}
}

// �� ƽ���� ����Ǵ� �޼����Դϴ�.
void ALetsShootPlayerController::Tick(float DeltaTime)
{
	// ���� Ŭ������ Tick �޼��带 ȣ���մϴ�.
	Super::Tick(DeltaTime);

	// HUD�� �ð��� �����ϰ�, �ð� ����ȭ�� Ȯ���մϴ�.
	SetHUDTime();
	CheckTimeSync(DeltaTime);

	// �ʱ�ȭ ���¸� Ȯ���մϴ�.
	PollInit();

	// ��Ʈ��ũ �����ð��� Ȯ���մϴ�.
	CheckPing(DeltaTime);
}


// ��Ʈ��ũ �����ð� (��)�� Ȯ���ϴ� �޼����Դϴ�.
void ALetsShootPlayerController::CheckPing(float DeltaTime)
{
	// ���� ������ �ִ� ��� �� �޼��带 �����մϴ�.
	if (HasAuthority()) return;

	// �� üũ�� ���� ���� �ð��� ������ŵ�ϴ�.
	HighPingRunningTime += DeltaTime;

	// �� üũ �ֱⰡ �Ǿ����� Ȯ���մϴ�.
	if (HighPingRunningTime > CheckPingFrequency)
	{
		// PlayerState�� �������ų� ���� PlayerState�� ĳ�����մϴ�.
		PlayerState = PlayerState == nullptr ? GetPlayerState<APlayerState>() : PlayerState;

		// PlayerState�� ��ȿ�� ���, ���� Ȯ���մϴ�.
		if (PlayerState)
		{
			// ���� �ʹ� ���� ��� ��� ����ϰ�, ���¸� ������ �����մϴ�.
			if (PlayerState->GetPing() * 4 > HighPingThreshold) // ���� ����Ǿ� �ֽ��ϴ�. ���� ���� �� / 4 �Դϴ�.
			{
				HighPingWarning();
				PingAnimationRunningTime = 0.f;
				ServerReportPingStatus(true);
			}
			else
			{
				// ���� ���� ���� ���, ���¸� ������ �����մϴ�.
				ServerReportPingStatus(false);
			}
		}

		// �� üũ�� ���� ���� �ð��� �ʱ�ȭ�մϴ�.
		HighPingRunningTime = 0.f;
	}

	// �� ��� �ִϸ��̼� ��� ���¸� Ȯ���մϴ�.
	bool bHighPingAnimationPlaying =
		LetsShootHUD && LetsShootHUD->CharacterOverlay &&
		LetsShootHUD->CharacterOverlay->HighPingAnimation &&
		LetsShootHUD->CharacterOverlay->IsAnimationPlaying(LetsShootHUD->CharacterOverlay->HighPingAnimation);

	// �� ��� �ִϸ��̼� ��� ���� ���, ���� �ð��� �˻��Ͽ� �����մϴ�.
	if (bHighPingAnimationPlaying)
	{
		PingAnimationRunningTime += DeltaTime;
		if (PingAnimationRunningTime > HighPingDuration)
		{
			StopHighPingWarning();
		}
	}
}

// ���� �޴��� ���ư��� �ɼ��� �����ִ� �޼����Դϴ�.
void ALetsShootPlayerController::ShowReturnToMainMenu()
{
	// ���� �޴� ������ ���� ��� �� �޼��带 �����մϴ�.
	if (ReturnToMainMenuWidget == nullptr) return;

	// ���� �޴��� �����ϰų� �����ϴ� �޴��� �����ɴϴ�.
	if (ReturnToMainMenu == nullptr)
	{
		ReturnToMainMenu = CreateWidget<UReturnToMainMenu>(this, ReturnToMainMenuWidget);
	}

	// ���� �޴��� ��ȿ�� ���, �޴��� ǥ���ϰų� �����մϴ�.
	if (ReturnToMainMenu)
	{
		bReturnToMainMenuOpen = !bReturnToMainMenuOpen;
		if (bReturnToMainMenuOpen)
		{
			ReturnToMainMenu->MenuSetup();
		}
		else
		{
			ReturnToMainMenu->MenuTearDown();
		}
	}
}


// �� ���� ǥ�� ���¸� �����ϴ� �޼����Դϴ�.
void ALetsShootPlayerController::OnRep_ShowTeamScores()
{
	// bShowTeamScores ���� ���� �� ������ �ʱ�ȭ�ϰų� ����ϴ�.
	if (bShowTeamScores)
	{
		InitTeamScores();  // �� ������ �ʱ�ȭ�մϴ�.
	}
	else
	{
		HideTeamScores();  // �� ������ ����ϴ�.
	}
}

// ���� �ʹ� ������ ������ �����ϴ� �޼����Դϴ�.
void ALetsShootPlayerController::ServerReportPingStatus_Implementation(bool bHighPing)  // bHighPing: ���� ������ �������� ��Ÿ���� �Ҹ����Դϴ�.
{
	HighPingDelegate.Broadcast(bHighPing);  // ���� �� ���¸� ��ε�ĳ��Ʈ�մϴ�.
}

// �������� �ð� ����ȭ�� Ȯ���ϴ� �޼����Դϴ�.
void ALetsShootPlayerController::CheckTimeSync(float DeltaTime)  // DeltaTime: ���� �����ӿ��� ���� �����ӱ��� �ɸ� �ð��Դϴ�.
{
	TimeSyncRunningTime += DeltaTime;

	// ���� ��Ʈ�ѷ��̰�, ����ȭ �ֱⰡ ���������, ������ ���� �ð��� ��û�մϴ�.
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;  // ����ȭ�� ���� ���� �ð��� �ʱ�ȭ�մϴ�.
	}
}

// ���� �� ��� ǥ���ϴ� �޼����Դϴ�.
void ALetsShootPlayerController::HighPingWarning()
{
	// LetsShootHUD�� �������ų� ���� HUD�� ĳ�����մϴ�.
	LetsShootHUD = LetsShootHUD == nullptr ? Cast<ALetsShootHUD>(GetHUD()) : LetsShootHUD;

	// LetsShootHUD�� ��ȿ�ϸ�, ���� �� �ִϸ��̼��� ǥ���մϴ�.
	bool bHUDValid = LetsShootHUD &&
		LetsShootHUD->CharacterOverlay &&
		LetsShootHUD->CharacterOverlay->HighPingImage &&
		LetsShootHUD->CharacterOverlay->HighPingAnimation;

	if (bHUDValid)
	{
		LetsShootHUD->CharacterOverlay->HighPingImage->SetOpacity(1.f);  // �� �̹����� �������� �����մϴ�.

		// �� �ִϸ��̼��� ����մϴ�.
		LetsShootHUD->CharacterOverlay->PlayAnimation(
			LetsShootHUD->CharacterOverlay->HighPingAnimation,
			0.f,
			5);
	}
}


// ���� �� ��� �����ϴ� �޼����Դϴ�.
void ALetsShootPlayerController::StopHighPingWarning()
{
	// LetsShootHUD�� �������ų� ���� HUD�� ĳ�����մϴ�.
	LetsShootHUD = LetsShootHUD == nullptr ? Cast<ALetsShootHUD>(GetHUD()) : LetsShootHUD;
	bool bHUDValid = LetsShootHUD &&
		LetsShootHUD->CharacterOverlay &&
		LetsShootHUD->CharacterOverlay->HighPingImage &&
		LetsShootHUD->CharacterOverlay->HighPingAnimation;

	if (bHUDValid)
	{
		LetsShootHUD->CharacterOverlay->HighPingImage->SetOpacity(0.f);  // �� �̹����� �������� 0���� �����մϴ�.

		if (LetsShootHUD->CharacterOverlay->IsAnimationPlaying(LetsShootHUD->CharacterOverlay->HighPingAnimation))
		{
			// ���� �� �ִϸ��̼��� ��� ���̸� �����մϴ�.
			LetsShootHUD->CharacterOverlay->StopAnimation(LetsShootHUD->CharacterOverlay->HighPingAnimation);
		}
	}
}

// ���� ����� ���¸� üũ�ϴ� �޼����Դϴ�.
void ALetsShootPlayerController::ServerCheckMatchState_Implementation()
{
	ALetsShootGameMode* GameMode = Cast<ALetsShootGameMode>(UGameplayStatics::GetGameMode(this));  // ���� ���� ��带 �����ɴϴ�.

	if (GameMode)
	{
		// ���� ���κ��� ������ ���� ���� ������ �����մϴ�.
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		CooldownTime = GameMode->CooldownTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		MatchState = GameMode->GetMatchState();

		ClientJoinMidgame(MatchState, WarmupTime, MatchTime, CooldownTime, LevelStartingTime);
	}
}

// Ŭ���̾�Ʈ�� �߰��� ���ӿ� �����ϴ� ��츦 ó���ϴ� �޼����Դϴ�.
void ALetsShootPlayerController::ClientJoinMidgame_Implementation(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime)
{
	// �� �Ű����� ���� �����մϴ�.
	WarmupTime = Warmup;
	MatchTime = Match;
	CooldownTime = Cooldown;
	LevelStartingTime = StartingTime;
	MatchState = StateOfMatch;

	OnMatchStateSet(MatchState);  // ���� ���¸� �����մϴ�.

	if (LetsShootHUD && MatchState == MatchState::WaitingToStart)
	{
		LetsShootHUD->AddAnnouncement();  // ������ ���� ��� ���̸� �˸��� �߰��մϴ�.
	}
}

// ���ο� Pawn�� ������ �� ȣ��Ǵ� �޼����Դϴ�.
void ALetsShootPlayerController::OnPossess(APawn* InPawn)  // InPawn: ������ ���ο� Pawn�Դϴ�.
{
	Super::OnPossess(InPawn);  // �θ� Ŭ������ OnPossess �޼��带 ȣ���մϴ�.

	ALetsShootCharacter* LetsShootCharacter = Cast<ALetsShootCharacter>(InPawn);  // InPawn�� ALetsShootCharacter�� ĳ�����մϴ�.

	if (LetsShootCharacter)
	{
		SetHUDHealth(LetsShootCharacter->GetHealth(), LetsShootCharacter->GetMaxHealth());  // HUD�� ü���� �����մϴ�.
	}
}


// HUD�� ü���� �����ϴ� �޼����Դϴ�.
void ALetsShootPlayerController::SetHUDHealth(float Health, float MaxHealth) // Health: ������ ü�� ��, MaxHealth: ü���� �ִ� ��
{
	// LetsShootHUD�� �������ų� ���� HUD�� ĳ�����մϴ�.
	LetsShootHUD = LetsShootHUD == nullptr ? Cast<ALetsShootHUD>(GetHUD()) : LetsShootHUD;

	bool bHUDValid = LetsShootHUD &&
		LetsShootHUD->CharacterOverlay &&
		LetsShootHUD->CharacterOverlay->HealthBar &&
		LetsShootHUD->CharacterOverlay->HealthText;

	if (bHUDValid)
	{
		// ü���� �ۼ�Ʈ�� ����ϰ�, HealthBar�� HealthText�� ������Ʈ�մϴ�.
		const float HealthPercent = Health / MaxHealth;
		LetsShootHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		LetsShootHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
	else
	{
		// ���� HUD�� �ʱ�ȭ���� �ʾҴٸ�, �ʱ�ȭ�� �ؾ��ϴ� ������ ǥ���ϰ� ü�� ���� �����մϴ�.
		bInitializeHealth = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
	}
}

// HUD�� ���� �����ϴ� �޼����Դϴ�.
void ALetsShootPlayerController::SetHUDShield(float Shield, float MaxShield) // Shield: ������ �� ��, MaxShield: ���� �ִ� ��
{
	// LetsShootHUD�� �������ų� ���� HUD�� ĳ�����մϴ�.
	LetsShootHUD = LetsShootHUD == nullptr ? Cast<ALetsShootHUD>(GetHUD()) : LetsShootHUD;

	bool bHUDValid = LetsShootHUD &&
		LetsShootHUD->CharacterOverlay &&
		LetsShootHUD->CharacterOverlay->ShieldBar &&
		LetsShootHUD->CharacterOverlay->ShieldText;

	if (bHUDValid)
	{
		// ���� �ۼ�Ʈ�� ����ϰ�, ShieldBar�� ShieldText�� ������Ʈ�մϴ�.
		const float ShieldPercent = Shield / MaxShield;
		LetsShootHUD->CharacterOverlay->ShieldBar->SetPercent(ShieldPercent);
		FString ShieldText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Shield), FMath::CeilToInt(MaxShield));
		LetsShootHUD->CharacterOverlay->ShieldText->SetText(FText::FromString(ShieldText));
	}
	else
	{
		// ���� HUD�� �ʱ�ȭ���� �ʾҴٸ�, �ʱ�ȭ�� �ؾ��ϴ� ������ ǥ���ϰ� �� ���� �����մϴ�.
		bInitializeShield = true;
		HUDShield = Shield;
		HUDMaxShield = MaxShield;
	}
}

// HUD�� ������ �����ϴ� �޼����Դϴ�.
void ALetsShootPlayerController::SetHUDScore(float Score) // Score: ������ ���� ��
{
	// LetsShootHUD�� �������ų� ���� HUD�� ĳ�����մϴ�.
	LetsShootHUD = LetsShootHUD == nullptr ? Cast<ALetsShootHUD>(GetHUD()) : LetsShootHUD;

	bool bHUDValid = LetsShootHUD &&
		LetsShootHUD->CharacterOverlay &&
		LetsShootHUD->CharacterOverlay->ScoreAmount;

	if (bHUDValid)
	{
		// ScoreAmount�� ������Ʈ�մϴ�.
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		LetsShootHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
	else
	{
		// ���� HUD�� �ʱ�ȭ���� �ʾҴٸ�, �ʱ�ȭ�� �ؾ��ϴ� ������ ǥ���ϰ� ���� ���� �����մϴ�.
		bInitializeScore = true;
		HUDScore = Score;
	}
}


// HUD�� Defeats ������ �����ϴ� �޼����Դϴ�.
void ALetsShootPlayerController::SetHUDDefeats(int32 Defeats) // Defeats: ������ Defeats ��
{
	// LetsShootHUD�� �������ų� ���� HUD�� ĳ�����մϴ�.
	LetsShootHUD = LetsShootHUD == nullptr ? Cast<ALetsShootHUD>(GetHUD()) : LetsShootHUD;

	bool bHUDValid = LetsShootHUD &&
		LetsShootHUD->CharacterOverlay &&
		LetsShootHUD->CharacterOverlay->DefeatsAmount;

	if (bHUDValid)
	{
		// DefeatsAmount�� ������Ʈ�մϴ�.
		FString DefeatsText = FString::Printf(TEXT("%d"), Defeats);
		LetsShootHUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));
	}
	else
	{
		// ���� HUD�� �ʱ�ȭ���� �ʾҴٸ�, �ʱ�ȭ�� �ؾ��ϴ� ������ ǥ���ϰ� Defeats ���� �����մϴ�.
		bInitializeDefeats = true;
		HUDDefeats = Defeats;
	}
}

// HUD�� ������ ź���� �����ϴ� �޼����Դϴ�.
void ALetsShootPlayerController::SetHUDWeaponAmmo(int32 Ammo) // Ammo: ������ ������ ź�� ��
{
	// LetsShootHUD�� �������ų� ���� HUD�� ĳ�����մϴ�.
	LetsShootHUD = LetsShootHUD == nullptr ? Cast<ALetsShootHUD>(GetHUD()) : LetsShootHUD;

	bool bHUDValid = LetsShootHUD &&
		LetsShootHUD->CharacterOverlay &&
		LetsShootHUD->CharacterOverlay->WeaponAmmoAmount;

	if (bHUDValid)
	{
		// WeaponAmmoAmount�� ������Ʈ�մϴ�.
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		LetsShootHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
	else
	{
		// ���� HUD�� �ʱ�ȭ���� �ʾҴٸ�, �ʱ�ȭ�� �ؾ��ϴ� ������ ǥ���ϰ� ������ ź�� ���� �����մϴ�.
		bInitializeWeaponAmmo = true;
		HUDWeaponAmmo = Ammo;
	}
}

// HUD�� ������ ź���� �����ϴ� �޼����Դϴ�.
void ALetsShootPlayerController::SetHUDCarriedAmmo(int32 Ammo) // Ammo: ������ ������ ź�� ��
{
	// LetsShootHUD�� �������ų� ���� HUD�� ĳ�����մϴ�.
	LetsShootHUD = LetsShootHUD == nullptr ? Cast<ALetsShootHUD>(GetHUD()) : LetsShootHUD;

	bool bHUDValid = LetsShootHUD &&
		LetsShootHUD->CharacterOverlay &&
		LetsShootHUD->CharacterOverlay->CarriedAmmoAmount;

	if (bHUDValid)
	{
		// CarriedAmmoAmount�� ������Ʈ�մϴ�.
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		LetsShootHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(AmmoText));
	}
	else
	{
		// ���� HUD�� �ʱ�ȭ���� �ʾҴٸ�, �ʱ�ȭ�� �ؾ��ϴ� ������ ǥ���ϰ� ������ ź�� ���� �����մϴ�.
		bInitializeCarriedAmmo = true;
		HUDCarriedAmmo = Ammo;
	}
}

// HUD�� ��� ī��Ʈ�ٿ� ������ �����ϴ� �޼����Դϴ�.
void ALetsShootPlayerController::SetHUDMatchCountdown(float CountdownTime) // CountdownTime: ������ ī��Ʈ�ٿ� �ð�(��)
{
	// LetsShootHUD�� �������ų� ���� HUD�� ĳ�����մϴ�.
	LetsShootHUD = LetsShootHUD == nullptr ? Cast<ALetsShootHUD>(GetHUD()) : LetsShootHUD;

	bool bHUDValid = LetsShootHUD &&
		LetsShootHUD->CharacterOverlay &&
		LetsShootHUD->CharacterOverlay->MatchCountdownText;

	if (bHUDValid)
	{
		if (CountdownTime < 0.f)
		{
			// ī��Ʈ�ٿ� �ð��� 0 �̸��� ��� �ؽ�Ʈ�� ���ϴ�.
			LetsShootHUD->CharacterOverlay->MatchCountdownText->SetText(FText());
			return;
		}

		// �а� �ʷ� ��ȯ�մϴ�.
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;

		// ī��Ʈ�ٿ� �ؽ�Ʈ�� ������Ʈ�մϴ�.
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		LetsShootHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
	}
}

// HUD�� ���� ī��Ʈ�ٿ� ������ �����ϴ� �޼����Դϴ�.
void ALetsShootPlayerController::SetHUDAnnouncementCountdown(float CountdownTime) // CountdownTime: ������ ī��Ʈ�ٿ� �ð�(��)
{
	// LetsShootHUD�� �������ų� ���� HUD�� ĳ�����մϴ�.
	LetsShootHUD = LetsShootHUD == nullptr ? Cast<ALetsShootHUD>(GetHUD()) : LetsShootHUD;

	bool bHUDValid = LetsShootHUD &&
		LetsShootHUD->Announcement &&
		LetsShootHUD->Announcement->WarmupTime;

	if (bHUDValid)
	{
		if (CountdownTime < 0.f)
		{
			// ī��Ʈ�ٿ� �ð��� 0 �̸��� ��� �ؽ�Ʈ�� ���ϴ�.
			LetsShootHUD->Announcement->WarmupTime->SetText(FText());
			return;
		}

		// �а� �ʷ� ��ȯ�մϴ�.
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;

		// ī��Ʈ�ٿ� �ؽ�Ʈ�� ������Ʈ�մϴ�.
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		LetsShootHUD->Announcement->WarmupTime->SetText(FText::FromString(CountdownText));
	}
}

// HUD�� ����ź ���� �����ϴ� �޼����Դϴ�.
void ALetsShootPlayerController::SetHUDGrenades(int32 Grenades) // Grenades: ������ ����ź�� ��
{
	// LetsShootHUD�� �������ų� ���� HUD�� ĳ�����մϴ�.
	LetsShootHUD = LetsShootHUD == nullptr ? Cast<ALetsShootHUD>(GetHUD()) : LetsShootHUD;

	bool bHUDValid = LetsShootHUD &&
		LetsShootHUD->CharacterOverlay &&
		LetsShootHUD->CharacterOverlay->GrenadesText;

	if (bHUDValid)
	{
		// ����ź�� ���� �ؽ�Ʈ�� ��ȯ�մϴ�.
		FString GrenadesText = FString::Printf(TEXT("%d"), Grenades);
		LetsShootHUD->CharacterOverlay->GrenadesText->SetText(FText::FromString(GrenadesText));
	}
	else
	{
		// ����ź�� ���� �ʱ�ȭ�մϴ�.
		bInitializeGrenades = true;
		HUDGrenades = Grenades;
	}
}


// HUD�� ���� �ð��� �����ϴ� �޼����Դϴ�.
void ALetsShootPlayerController::SetHUDTime()
{
	float TimeLeft = 0.f;

	// ���� ���� ���¿� ���� ���� �ð��� ����մϴ�.
	if (MatchState == MatchState::WaitingToStart) TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::InProgress) TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	else if (MatchState == MatchState::Cooldown) TimeLeft = CooldownTime + WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;

	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft); // ���� �ʷ� ��ȯ�մϴ�.

	if (HasAuthority()) // �������� ����Ǵ� ���
	{
		if (LetsShootGameMode == nullptr)
		{
			// ���� ��带 �������ų� ���� ���� ��带 ĳ�����մϴ�.
			LetsShootGameMode = Cast<ALetsShootGameMode>(UGameplayStatics::GetGameMode(this));
			LevelStartingTime = LetsShootGameMode->LevelStartingTime;
		}
		LetsShootGameMode = LetsShootGameMode == nullptr ? Cast<ALetsShootGameMode>(UGameplayStatics::GetGameMode(this)) : LetsShootGameMode;
		if (LetsShootGameMode)
		{
			SecondsLeft = FMath::CeilToInt(LetsShootGameMode->GetCountdownTime() + LevelStartingTime);
		}
	}

	if (CountdownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
		{
			// ���� ī��Ʈ�ٿ��� �����մϴ�.
			SetHUDAnnouncementCountdown(TimeLeft);
		}
		if (MatchState == MatchState::InProgress)
		{
			// ��� ī��Ʈ�ٿ��� �����մϴ�.
			SetHUDMatchCountdown(TimeLeft);
		}
	}

	CountdownInt = SecondsLeft;
}

// �ʱ�ȭ ��û�� Ȯ���ϴ� �޼����Դϴ�.
void ALetsShootPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (LetsShootHUD && LetsShootHUD->CharacterOverlay)
		{
			CharacterOverlay = LetsShootHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				// �� ��Ұ� �ʱ�ȭ�Ǿ�� �ϴ� ��� �ش� ����� HUD�� �����մϴ�.
				if (bInitializeHealth) SetHUDHealth(HUDHealth, HUDMaxHealth);
				if (bInitializeShield) SetHUDShield(HUDShield, HUDMaxShield);
				if (bInitializeScore) SetHUDScore(HUDScore);
				if (bInitializeDefeats) SetHUDDefeats(HUDDefeats);
				if (bInitializeCarriedAmmo) SetHUDCarriedAmmo(HUDCarriedAmmo);
				if (bInitializeWeaponAmmo) SetHUDWeaponAmmo(HUDWeaponAmmo);

				ALetsShootCharacter* LetsShootCharacter = Cast<ALetsShootCharacter>(GetPawn());
				if (LetsShootCharacter && LetsShootCharacter->GetCombat())
				{
					if (bInitializeGrenades) SetHUDGrenades(LetsShootCharacter->GetCombat()->GetGrenades());
				}
			}
		}
	}
}


// �Է� ������Ҹ� �����ϴ� �޼����Դϴ�.
void ALetsShootPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent(); // �θ� Ŭ������ SetupInputComponent �޼��带 ȣ���մϴ�.
	if (InputComponent == nullptr) return; // �Է� ������Ұ� ���� ���, �� �޼��带 �����մϴ�.

	// "Quit" �׼ǿ� ���� ShowReturnToMainMenu �޼��带 ���ε��մϴ�.
	InputComponent->BindAction("Quit", IE_Pressed, this, &ALetsShootPlayerController::ShowReturnToMainMenu);
}

// ������ ���� �ð� ��û�� ������ �޼����Դϴ�.
void ALetsShootPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds(); // ������ ���� �ð��� ����ϴ�.
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt); // Ŭ���̾�Ʈ���� �����մϴ�.
}

// Ŭ���̾�Ʈ���� ���� �ð��� �����ϴ� �޼����Դϴ�.
void ALetsShootPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest; // �պ� �ð��� ����մϴ�.
	SingleTripTime = 0.5f * RoundTripTime; // �ܹ��� �ð��� ����մϴ�.
	float CurrentServerTime = TimeServerReceivedClientRequest + SingleTripTime; // ������ ���� �ð��� ����մϴ�.
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds(); // Ŭ���̾�Ʈ�� ���� ������ �ð� ���̸� �����մϴ�.
}

// ������ ���� �ð��� ��� �޼����Դϴ�.
float ALetsShootPlayerController::GetServerTime()
{
	if (HasAuthority()) return GetWorld()->GetTimeSeconds(); // ������� ���� �ð��� �ٷ� ��ȯ�մϴ�.
	else return GetWorld()->GetTimeSeconds() + ClientServerDelta; // Ŭ���̾�Ʈ��� �ð� ���̸� ���� �ð��� ��ȯ�մϴ�.
}

// �÷��̾ �޾��� �� ȣ��Ǵ� �޼����Դϴ�.
void ALetsShootPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer(); // �θ� Ŭ������ ReceivedPlayer �޼��带 ȣ���մϴ�.
	if (IsLocalController()) // ���� ��Ʈ�ѷ��� ���
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds()); // ������ ���� �ð��� ��û�մϴ�.
	}
}

// ��ġ ���°� �����Ǿ��� �� ȣ��Ǵ� �޼����Դϴ�.
void ALetsShootPlayerController::OnMatchStateSet(FName State, bool bTeamsMatch)
{
	MatchState = State; // ��ġ ���¸� �����մϴ�.

	// ��ġ ���¿� ���� �޼��带 ȣ���մϴ�.
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted(bTeamsMatch);
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

// ��ġ ���°� ����Ǿ��� �� ȣ��Ǵ� �޼����Դϴ�.
void ALetsShootPlayerController::OnRep_MatchState()
{
	// ��ġ ���¿� ���� �޼��带 ȣ���մϴ�.
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}


// ��ġ�� ���۵Ǿ��� �� ó���ϴ� �޼����Դϴ�.
void ALetsShootPlayerController::HandleMatchHasStarted(bool bTeamsMatch) // bTeamsMatch: �� ��ġ ���θ� ��Ÿ���� �Ҹ��� ���Դϴ�.
{
	if (HasAuthority()) bShowTeamScores = bTeamsMatch; // ���� ������ ������, �� ���� ǥ�� ���θ� �����մϴ�.
	LetsShootHUD = LetsShootHUD == nullptr ? Cast<ALetsShootHUD>(GetHUD()) : LetsShootHUD; // LetsShootHUD ��ü�� �ʱ�ȭ�մϴ�.

	if (LetsShootHUD) // LetsShootHUD�� ��ȿ�� ���
	{
		if (LetsShootHUD->CharacterOverlay == nullptr) LetsShootHUD->AddCharacterOverlay(); // ĳ���� �������̰� ������ �߰��մϴ�.
		if (LetsShootHUD->Announcement)
		{
			LetsShootHUD->Announcement->SetVisibility(ESlateVisibility::Hidden); // ���������� ����ϴ�.
		}
		if (!HasAuthority()) return; // ���� ������ ������ �޼��带 �����մϴ�.

		if (bTeamsMatch)
		{
			InitTeamScores(); // �� ������ �ʱ�ȭ�մϴ�.
		}
		else
		{
			HideTeamScores(); // �� ������ ����ϴ�.
		}
	}
}

// ��ٿ��� ó���ϴ� �޼����Դϴ�.
void ALetsShootPlayerController::HandleCooldown()
{
	LetsShootHUD = LetsShootHUD == nullptr ? Cast<ALetsShootHUD>(GetHUD()) : LetsShootHUD; // LetsShootHUD ��ü�� �ʱ�ȭ�մϴ�.

	if (LetsShootHUD) // LetsShootHUD�� ��ȿ�� ���
	{
		LetsShootHUD->CharacterOverlay->RemoveFromParent(); // ĳ���� �������̸� �����մϴ�.
		bool bHUDValid = LetsShootHUD->Announcement &&
			LetsShootHUD->Announcement->AnnouncementText &&
			LetsShootHUD->Announcement->InfoText; // HUD ��ȿ���� �˻��մϴ�.

		if (bHUDValid) // HUD�� ��ȿ�� ���
		{
			LetsShootHUD->Announcement->SetVisibility(ESlateVisibility::Visible); // ���������� ǥ���մϴ�.
			FString AnnouncementText = Announcement::NewMatchStartsIn; // �������� �ؽ�Ʈ�� �����մϴ�.
			LetsShootHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText)); // �ؽ�Ʈ�� HUD�� �����մϴ�.

			ALetsShootGameState* LetsShootGameState = Cast<ALetsShootGameState>(UGameplayStatics::GetGameState(this));
			ALetsShootPlayerState* LetsShootPlayerState = GetPlayerState<ALetsShootPlayerState>();
			if (LetsShootGameState && LetsShootPlayerState) // ���� ���¿� �÷��̾� ���°� ��ȿ�� ���
			{
				TArray<ALetsShootPlayerState*> TopPlayers = LetsShootGameState->TopScoringPlayers; // �ְ� ������ ����� ����ϴ�.
				FString InfoTextString = bShowTeamScores ? GetTeamsInfoText(LetsShootGameState) : GetInfoText(TopPlayers); // ���� �ؽ�Ʈ�� ����ϴ�.

				LetsShootHUD->Announcement->InfoText->SetText(FText::FromString(InfoTextString)); // ���� �ؽ�Ʈ�� HUD�� �����մϴ�.
			}
		}
	}

	ALetsShootCharacter* LetsShootCharacter = Cast<ALetsShootCharacter>(GetPawn()); // �÷��̾� ĳ���͸� ����ϴ�.
	if (LetsShootCharacter && LetsShootCharacter->GetCombat()) // ĳ���Ϳ� ���� ���°� ��ȿ�� ���
	{
		LetsShootCharacter->bDisableGameplay = true; // �����÷��̸� ��Ȱ��ȭ�մϴ�.
		LetsShootCharacter->GetCombat()->FireButtonPressed(false); // �߻� ��ư�� ��Ȱ��ȭ�մϴ�.
	}
}


// �־��� �÷��̾� ��Ͽ� ���� ���� �ؽ�Ʈ�� �������� �޼����Դϴ�.
FString ALetsShootPlayerController::GetInfoText(const TArray<class ALetsShootPlayerState*>& Players) // Players: ���� �ؽ�Ʈ�� ���� �÷��̾� ���� �迭�Դϴ�.
{
	ALetsShootPlayerState* LetsShootPlayerState = GetPlayerState<ALetsShootPlayerState>(); // ���� �÷��̾� ���¸� �����ɴϴ�.
	if (LetsShootPlayerState == nullptr) return FString(); // �÷��̾� ���°� ������ �� ���ڿ��� ��ȯ�մϴ�.
	FString InfoTextString; // ��ȯ�� ���� �ؽ�Ʈ�� �����ϴ� ���ڿ��Դϴ�.

	if (Players.Num() == 0) // �÷��̾ ���� ���
	{
		InfoTextString = Announcement::ThereIsNoWinner; // ����� ���� �޽����� �����մϴ�.
	}
	else if (Players.Num() == 1 && Players[0] == LetsShootPlayerState) // ���� �÷��̾ ����� ���
	{
		InfoTextString = Announcement::YouAreTheWinner; // ��� �޽����� �����մϴ�.
	}
	else if (Players.Num() == 1) // �ٸ� �÷��̾ ����� ���
	{
		InfoTextString = FString::Printf(TEXT("Winner: \n%s"), *Players[0]->GetPlayerName()); // ������� �̸��� ������ �޽����� �����մϴ�.
	}
	else if (Players.Num() > 1) // ���� �÷��̾ ����� ���
	{
		InfoTextString = Announcement::PlayersTiedForTheWin; // ����� ���� �޽����� �����մϴ�.
		InfoTextString.Append(FString("\n"));
		for (auto TiedPlayer : Players) // ����� �� �÷��̾ ����
		{
			InfoTextString.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetPlayerName())); // �÷��̾��� �̸��� ���� �ؽ�Ʈ�� �߰��մϴ�.
		}
	}

	return InfoTextString; // ���� �ؽ�Ʈ�� ��ȯ�մϴ�.
}

// �־��� ���� ���¿� ���� �� ���� �ؽ�Ʈ�� �������� �޼����Դϴ�.
FString ALetsShootPlayerController::GetTeamsInfoText(ALetsShootGameState* LetsShootGameState) // LetsShootGameState: �� ���� �ؽ�Ʈ�� ���� ���� �����Դϴ�.
{
	if (LetsShootGameState == nullptr) return FString(); // ���� ���°� ������ �� ���ڿ��� ��ȯ�մϴ�.
	FString InfoTextString; // ��ȯ�� ���� �ؽ�Ʈ�� �����ϴ� ���ڿ��Դϴ�.

	const int32 RedTeamScore = LetsShootGameState->RedTeamScore; // ���� ���� �����Դϴ�.
	const int32 BlueTeamScore = LetsShootGameState->BlueTeamScore; // ��� ���� �����Դϴ�.

	if (RedTeamScore == 0 && BlueTeamScore == 0) // �� �� ��� ������ ���� ���
	{
		InfoTextString = Announcement::ThereIsNoWinner; // ����� ���� �޽����� �����մϴ�.
	}
	else if (RedTeamScore == BlueTeamScore) // �� ���� ������ ���� ���
	{
		InfoTextString = FString::Printf(TEXT("%s\n"), *Announcement::TeamsTiedForTheWin); // �� ���� �޽����� �����մϴ�.
		InfoTextString.Append(Announcement::RedTeam);
		InfoTextString.Append(TEXT("\n"));
		InfoTextString.Append(Announcement::BlueTeam);
		InfoTextString.Append(TEXT("\n"));
	}
	else if (RedTeamScore > BlueTeamScore) // ���� ���� �̱� ���
	{
		InfoTextString = Announcement::RedTeamWins; // ���� �� ��� �޽����� �����մϴ�.
		InfoTextString.Append(TEXT("\n"));
		InfoTextString.Append(FString::Printf(TEXT("%s: %d\n"), *Announcement::RedTeam, RedTeamScore)); // ���� ���� ������ ���� �ؽ�Ʈ�� �߰��մϴ�.
		InfoTextString.Append(FString::Printf(TEXT("%s: %d\n"), *Announcement::BlueTeam, BlueTeamScore)); // ��� ���� ������ ���� �ؽ�Ʈ�� �߰��մϴ�.
	}
	else if (BlueTeamScore > RedTeamScore) // ��� ���� �̱� ���
	{
		InfoTextString = Announcement::BlueTeamWins; // ��� �� ��� �޽����� �����մϴ�.
		InfoTextString.Append(TEXT("\n"));
		InfoTextString.Append(FString::Printf(TEXT("%s: %d\n"), *Announcement::BlueTeam, BlueTeamScore)); // ��� ���� ������ ���� �ؽ�Ʈ�� �߰��մϴ�.
		InfoTextString.Append(FString::Printf(TEXT("%s: %d\n"), *Announcement::RedTeam, RedTeamScore)); // ���� ���� ������ ���� �ؽ�Ʈ�� �߰��մϴ�.
	}

	return InfoTextString; // ���� �ؽ�Ʈ�� ��ȯ�մϴ�.
}


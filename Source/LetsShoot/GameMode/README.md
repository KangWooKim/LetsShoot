# LetsShootGameMode.h

### ALetsShootGameMode::ALetsShootGameMode(): 
이 메서드는 ALetsShootGameMode 클래스의 생성자입니다. 객체가 생성될 때 이 메서드가 호출되며, 이 클래스의 변수들을 초기화합니다. 여기서는 bDelayedStart 변수를 true로 설정하여 게임 시작을 지연시킵니다.

### void ALetsShootGameMode::BeginPlay(): 
이 메서드는 Actor가 게임 플레이를 시작할 때 호출됩니다. 여기서는 LevelStartingTime을 현재 세계 시간으로 설정합니다.

### void ALetsShootGameMode::Tick(float DeltaTime): 
이 메서드는 매 프레임마다 호출되며, 게임의 상태에 따라 다른 동작을 수행합니다. WaitingToStart, InProgress, Cooldown 상태에 따라 다른 로직을 수행하며, 각 상태는 게임의 준비 단계, 진행 중인 단계, 그리고 쿨다운 단계를 나타냅니다.

### void ALetsShootGameMode::OnMatchStateSet(): 
이 메서드는 매치 상태가 설정될 때 호출됩니다. 여기서는 모든 플레이어 컨트롤러에게 매치 상태가 설정되었음을 알립니다.

### float ALetsShootGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage): 
이 메서드는 공격자와 피해자 간의 데미지를 계산합니다. 현재는 기본 데미지를 그대로 반환합니다.

### void ALetsShootGameMode::PlayerEliminated(class ALetsShootCharacter* ElimmedCharacter, class ALetsShootPlayerController* VictimController, ALetsShootPlayerController* AttackerController): 
이 메서드는 플레이어가 제거되었을 때 호출됩니다. 여기서는 공격자에게 점수를 부여하고, 피해자에게 패배를 추가하며, 각 플레이어의 상태를 업데이트합니다.

### void ALetsShootGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController): 
이 메서드는 캐릭터의 재생성을 요청할 때 호출됩니다. 여기서는 제거된 캐릭터를 리셋하고 파괴한 후, 랜덤한 플레이어 시작 위치에서 캐릭터를 재생성합니다.

### void ALetsShootGameMode::PlayerLeftGame(ALetsShootPlayerState* PlayerLeaving): 
이 메서드는 플레이어가 게임을 떠날 때 호출됩니다.

---

# GetFlagGameMode.h

### void AGetFlagGameMode::PlayerEliminated(class ALetsShootCharacter* ElimmedCharacter, class ALetsShootPlayerController* VictimController, ALetsShootPlayerController* AttackerController): 
이 메서드는 부모 클래스인 "LetsShootGameMode"의 "PlayerEliminated" 메서드를 오버라이드합니다. 플레이어가 제거되었을 때 호출되며, 현재 구현에서는 부모 클래스의 동일 메서드를 호출하여 기본 로직을 수행합니다.

### void AGetFlagGameMode::FlagCaptured(AFlag* Flag, AFlagZone* Zone):
이 메서드는 깃발이 집었을 때 호출됩니다. 깃발을 가져온 팀이 자신의 팀과 다른 경우에만 유효한 득점으로 판단합니다. 그리고, 깃발을 포착한 팀에 따라 해당 팀의 점수를 증가시킵니다. 이를 위해 "LetsShootGameState" 클래스의 메서드인 "BlueTeamScores" 또는 "RedTeamScores"를 호출합니다.

---

# LobbyGameMode.h

### void ALobbyGameMode::PostLogin(APlayerController* NewPlayer): 
이 메서드는 플레이어가 게임에 로그인하면 호출되는 메서드입니다. 이 메서드는 여러 단계로 구성되어 있습니다.

### Super::PostLogin(NewPlayer): 
부모 클래스의 PostLogin 메서드를 호출합니다. 이는 기본적인 로그인 처리를 합니다.

### NumberOfPlayers: 
현재 게임 상태에 연결된 플레이어 수를 가져옵니다.

### UMultiplayerSessionsSubsystem* Subsystem: 
게임 인스턴스에서 멀티플레이어 세션 서브시스템을 가져옵니다. 이 서브시스템은 멀티플레이어 세션을 관리하는 데 사용됩니다.

if (NumberOfPlayers == Subsystem->DesiredNumPublicConnections): 현재 연결된 플레이어 수가 원하는 연결 수와 같은지 확인합니다. 같다면, 서버가 다른 맵으로 이동하도록 설정하고, 원하는 매치 타입에 따라 다른 맵으로 이동하도록 합니다. 이는 게임 모드가 'FreeForAll', 'Teams', 'CaptureTheFlag' 등에 따라 다르게 설정됩니다. 이를 위해 ServerTravel 메서드를 사용합니다. 이 메서드는 서버와 모든 클라이언트를 특정 맵으로 이동시키는 데 사용됩니다.

---

# TeamsGameMode.h

### ATeamsGameMode::ATeamsGameMode(): 
이 메서드는 'TeamsGameMode' 객체가 생성될 때 호출되는 생성자입니다. bTeamsMatch라는 변수를 참으로 설정하여, 이 게임 모드가 팀 기반임을 나타냅니다.

### void ATeamsGameMode::PostLogin(APlayerController* NewPlayer): 
플레이어가 게임에 로그인하면 호출되는 메서드입니다. 플레이어가 팀에 속하지 않은 상태라면, 현재 팀의 구성원 수를 확인하고 더 적은 구성원이 있는 팀에 플레이어를 배정합니다.

### void ATeamsGameMode::Logout(AController* Exiting): 
플레이어가 게임에서 로그아웃하면 호출되는 메서드입니다. 플레이어를 해당 팀에서 제거합니다.

### void ATeamsGameMode::HandleMatchHasStarted(): 
매치가 시작되었을 때 호출되는 메서드입니다. 팀에 배정되지 않은 플레이어를 찾아 더 적은 구성원이 있는 팀에 배정합니다.

### float ATeamsGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage): 
플레이어가 공격을 받았을 때 호출되는 메서드입니다. 공격자와 피해자의 팀이 같다면 데미지를 0으로 설정하여, 팀원에게는 데미지가 가지 않도록 합니다.

### void ATeamsGameMode::PlayerEliminated(ALetsShootCharacter* ElimmedCharacter, ALetsShootPlayerController* VictimController, ALetsShootPlayerController* AttackerController): 
플레이어가 죽었을 때 호출되는 메서드입니다. 공격자의 팀에 점수를 추가합니다.

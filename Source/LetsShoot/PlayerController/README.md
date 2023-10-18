# LetsShootPlayerController.h

### BroadcastElim: 

탈락자와 공격자 사이의 전달을 클라이언트로 전송하는 함수입니다.

### ClientElimAnnouncement_Implementation: 

공격자와 희생자 사이의 전달을 클라이언트에 알립니다.

### BeginPlay: 

게임 시작 시 HUD 초기화를 처리하고, 서버가 매치 상태를 확인합니다.

### GetLifetimeReplicatedProps: 

클래스의 복제된 속성을 정의해 줍니다.

### HideTeamScores: 

팀 점수를 화면에서 숨깁니다.

### InitTeamScores: 

HUD 상의 팀 점수를 초기화합니다.

### SetHUDRedTeamScore: 

HUD 상의 레드팀 점수를 설정합니다.

### SetHUDBlueTeamScore:

HUD 상의 블루팀 점수를 설정합니다.

### Tick:

매 프레임에서 HUD 시간을 갱신하고, 동기화를 체크하며, 초기화를 확인하고, 핑을 체크합니다.

### CheckPing: 

핑에 대한 정보를 확인하고, 높은 핑을 경고하는 함수입니다.

### ShowReturnToMainMenu: 

메인 메뉴로 돌아가는 위젯을 보여줍니다.

### OnRep_ShowTeamScores: 

팀 점수를 표시하거나 숨기는 함수를 호출합니다.

### ServerReportPingStatus_Implementation: 

서버에 핑 상태를 보고하고, HighPingDelegate를 Broadcast합니다.

### CheckTimeSync: 

로컬 컨트롤러에서 서버 시간 동기화를 요청하고, 시간을 재설정합니다.

### HighPingWarning: 

HUD의 높은 핑 경고 이미지 및 애니메이션을 활성화합니다.

### StopHighPingWarning: 

HUD의 높은 핑 경고 이미지 및 애니메이션을 중지합니다.

### ServerCheckMatchState_Implementation: 

서버와 동기화하고 클라이언트에 게임 상태 데이터를 전송합니다.

### ClientJoinMidgame_Implementation: 

서버의 게임 시간과 상태를 클라이언트에 초기화하고, 상태와 같은 관련 이벤트 설정합니다.

### OnPossess: 

캐릭터 소유 시 HUD에 건강 정보 초기화를 설정합니다.

### SetHUDHealth: 

HUD에 캐릭터의 현재 생명력과 최대 생명력을 표시합니다.

### SetHUDShield: 

HUD에 캐릭터의 현재 방어력과 최대 방어력을 표시합니다.

### SetHUDScore: 

HUD에 캐릭터의 스코어를 표시합니다.

### SetHUDDefeats: 

HUD에 캐릭터의 처치 수를 표시합니다.

### SetHUDWeaponAmmo: 

HUD에 현재 무기의 탄약량을 표시합니다.

### SetHUDCarriedAmmo: 

HUD에 캐릭터가 들고 있는 전체 탄약량을 표시합니다.

### SetHUDMatchCountdown: 

HUD에 매치 카운트다운을 표시합니다.

### SetHUDAnnouncementCountdown: 

HUD에게 남은 카운트다운 시간을 갱신해주는 메서드입니다(분:초 형식).

### SetHUDGrenades: 

HUD에 현재 플레이어의 수류탄 개수를 표시해주는 메서드입니다.

### SetHUDTime: 

현재 게임 상태(WaitingToStart, InProgress, Cooldown)에 따라 HUD에 표시할 시간을 설정하는 메서드입니다.

### PollInit: 

HUD에 설정할 초기값을 세팅해주며(property 및 method 호출), 필요한 값들이 설정되지 않았다면 다시 초기화하는 메서드입니다.

### SetupInputComponent: 

플레이어 컨트롤러에 대한 인풋 설정을 처리해주는 메서드입니다. 이 경우 "Quit" 액션을 바인딩하고 있습니다.

### ServerRequestServerTime_Implementation: 

클라이언트로부터 서버 시간을 요청받는 서버 메서드입니다.

### ClientReportServerTime_Implementation: 

서버로부터 받은 서버 시간을 클라이언트에게 전달해주는 메서드입니다.

### GetServerTime: 

현재 서버 시간을 반환하는 메서드입니다. (권한이 있다면 직접 반환하고, 그렇지 않다면 클라이언트 상에서 계산된 delta를 이용해 반환합니다)

### ReceivedPlayer: 

플레이어가 게임에 참여할 때 호출되는 메서드입니다. 이 메서드 내에서 서버 시간을 요청한다.

### OnMatchStateSet, OnRep_MatchState: 

게임 상태가 변경될 때(예, InProgress, Cooldown 등) 호출되는 메서드입니다. 상태 변경에 따라 처리해야 할 작업을 정의합니다.

### HandleMatchHasStarted, HandleCooldown: 

각각 게임이 시작될 때와 쿨다운 상태일 때, 필요한 처리를 수행하는 메서드입니다. 이 경우 HUD 및 팀 점수 설정 작업이 포함됩니다.

### HandleCooldown: 

쿨다운 상태일 때 HUD의 캐릭터 오버레이를 제거하고, 승자와 팀 점수에 따른 상황을 화면에 표시합니다. 플레이어의 행동 또한 비활성화됩니다.

### GetInfoText: 

플레이어 목록을 받아 상위 플레이어들의 순위 정보를 문자열로 반환합니다(플레이어 개인 승리, 공동 승리 등).

### GetTeamsInfoText:

게임 상태를 통해 팀 점수에 따른 결과를 문자열로 반환합니다(무승부, 특정 팀 승리 등).

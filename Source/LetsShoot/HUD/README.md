# LetsShootHUD.h

### void ALetsShootHUD::BeginPlay(): 
게임 플레이가 시작될 때 호출되는 메서드입니다. 이 클래스에서는 특별한 동작이 정의되지 않았습니다.

### void ALetsShootHUD::AddDieAnnouncement(FString Attacker, FString Victim): 
이 메서드는 게임 플레이어가 다른 플레이어를 제거했을 때 발생하는 'DieAnnouncement'이라는 이벤트를 처리합니다. DieAnnouncement 위젯을 생성하고, 이 위젯에 제거된 플레이어와 제거한 플레이어의 이름을 설정합니다. 이 위젯은 현재 뷰포트에 추가되고, 일정 시간 후에 제거됩니다.

### void ALetsShootHUD::DieAnnouncementTimerFinished(UDieAnnouncement* MsgToRemove): 
이 메서드는 DieAnnouncement 이벤트의 타이머가 종료될 때 호출됩니다. 이 메서드는 해당 메시지 위젯을 부모 위젯에서 제거합니다.

### void ALetsShootHUD::AddLetsShootCharacterOverlay():
이 메서드는 플레이어 캐릭터의 오버레이를 화면에 추가하는 기능을 담당합니다. LetsShootCharacterOverlay 위젯을 생성하고 현재 뷰포트에 추가합니다.

### void ALetsShootHUD::AddAnnouncement(): 
이 메서드는 게임 플레이 중 발생하는 중요한 이벤트를 표시하는 Announcement 위젯을 생성하고 현재 뷰포트에 추가합니다.

### void ALetsShootHUD::DrawHUD(): 
이 메서드는 HUD를 그리는 기능을 담당합니다. 여기서는 게임 뷰포트의 크기를 얻고, 크로스헤어의 위치와 색상을 설정하여 화면에 그립니다.

### void ALetsShootHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor): 
이 메서드는 특정 텍스처를 이용해 크로스헤어를 그리는 역할을 합니다. 크로스헤어의 위치는 뷰포트의 중심에서부터 특정 거리만큼 떨어진 위치에 그려집니다.

이 클래스는 게임 화면에 필요한 정보들을 표시하는 역할을 합니다. 그러므로, 게임 상황에 따른 다양한 이벤트 처리와 그에 따른 화면 출력을 담당하고 있습니다.

---

# DieAnnouncement.h

### void UDieAnnouncement::SetDieAnnouncementText(FString AttackerName, FString VictimName): 
이 메서드는 공격자와 피해자의 이름을 인수로 받아, "AttackerName elimmed VictimName!" 형식의 문자열을 생성합니다. 이 문자열은 이벤트 위젯의 'AnnouncementText'에 설정되어, 화면에 표시됩니다.
즉, 이 메서드는 특정 플레이어가 다른 플레이어를 제거한 이벤트가 발생했을 때, 해당 정보를 화면에 표시하는 역할을 합니다. 해당 이벤트를 처리하는 위젯의 텍스트를 업데이트하므로, 게임 플레이어는 어떤 플레이어가 다른 플레이어를 제거했는지를 확인할 수 있습니다.

---

# OverheadWidget.h

### void UOverheadWidget::SetDisplayText(FString TextToDisplay): 
이 메서드는 'TextToDisplay'라는 인수를 받아, 이를 위젯의 'DisplayText'에 설정합니다. 이로써, 주어진 문자열이 화면에 표시되게 됩니다.

### void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn): 
이 메서드는 'InPawn'이라는 인수를 받아, 해당 플레이어의 네트워크 역할(ENetRole)을 문자열로 변환하고 이를 화면에 표시합니다. 각 역할은 다음과 같습니다:

ROLE_Authority: 이 역할은 게임의 상태를 제어하는 서버에 할당됩니다.
ROLE_AutonomousProxy: 이 역할은 플레이어의 입장에서 자신을 제어하는 객체에 할당됩니다.
ROLE_SimulatedProxy: 이 역할은 네트워크에서 다른 플레이어의 행동을 시뮬레이션하는 객체에 할당됩니다.
ROLE_None: 이 역할은 네트워크 역할이 없는 객체에 할당됩니다.

### void UOverheadWidget::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld): 
이 메서드는 특정 레벨이 월드에서 제거될 때 호출됩니다. 이 메서드는 위젯을 부모 위젯에서 제거하고, 상위 클래스의 OnLevelRemovedFromWorld 메서드를 호출하여 추가적인 정리 작업을 수행합니다. 이 메서드는 주로 레벨이 바뀔 때 화면에 표시되는 위젯을 정리하는데 사용됩니다.

---

# BackToMenu

### MenuSetup()

메인 메뉴로 돌아가기 위한 UI 설정 및 버튼 등록

멀티플레이어 세션 시작과 종료를 처리하는 서브시스템과 연결


### Initialize()

메뉴가 정상적으로 초기화되었는지 확인


### OnDestroySession(bool bWasSuccessful)

세션 종료가 완료되면 호스트가 메인 메뉴로 돌아감

세션 종료가 성공하지 못하면 Return 버튼을 사용 가능하게 함


### MenuTearDown()

메뉴에서 나갈 때 설정 및 버튼 바인딩 해제

이벤트 제거 및 입력 모드 설정


### ReturnButtonClicked()

Return 버튼 클릭 시 실행

게임을 나가고 메인 메뉴로 돌아갈 수 있도록 처리


### OnPlayerLeftGame()

플레이어가 게임에서 나가면 실행

멀티플레이어 세션을 종료함

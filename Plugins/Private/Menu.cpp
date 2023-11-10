// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

// UMenu 클래스의 MenuSetup 메서드 정의
// 이 메서드는 메뉴 UI를 설정하고, 멀티플레이어 세션 서브시스템을 초기화합니다.
void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath)
{
	// 로비로의 경로를 설정합니다. 'listen' 쿼리 파라미터를 추가해서 서버가 듣기 상태가 되도록 합니다.
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);

	// 공개 연결 수와 매치 타입을 설정합니다.
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;

	// 뷰포트에 메뉴를 추가하고, 가시성을 설정합니다.
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	// 현재 세계를 가져와 플레이어 컨트롤러에 UI 전용 입력 모드를 설정합니다.
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	// 게임 인스턴스에서 멀티플레이어 세션 서브시스템을 가져옵니다.
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	// 멀티플레이어 세션 서브시스템이 있으면, 세션 관련 콜백을 연결합니다.
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
		MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
	}
}

// UMenu 클래스의 Initialize 메서드 정의
// 이 메서드는 UI 요소들이 제대로 초기화되었는지 확인하고, 각 버튼에 적절한 콜백을 설정합니다.
bool UMenu::Initialize()
{
	// 부모 클래스의 Initialize 메서드를 호출하여 기본 초기화를 수행합니다.
	if (!Super::Initialize())
	{
		return false;
	}

	// 호스트 버튼이 있으면, 클릭 이벤트에 HostButtonClicked 메서드를 연결합니다.
	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}
	// 조인 버튼이 있으면, 클릭 이벤트에 JoinButtonClicked 메서드를 연결합니다.
	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}

	// 초기화가 성공적으로 완료되면 true를 반환합니다.
	return true;
}


// UMenu 클래스의 NativeDestruct 메서드 정의
// 이 메서드는 UObject가 파괴될 때 호출되며, MenuTearDown을 호출한 후 기본 파괴자를 호출합니다.
void UMenu::NativeDestruct()
{
	// 메뉴를 해체하는 작업을 수행합니다.
	MenuTearDown();

	// 기본 클래스의 파괴자를 호출합니다.
	Super::NativeDestruct();
}

// UMenu 클래스의 OnCreateSession 메서드 정의
// 세션 생성 요청의 결과를 처리하는 메서드입니다. 성공하면 로비로 이동하고, 실패하면 오류 메시지를 표시합니다.
void UMenu::OnCreateSession(bool bWasSuccessful)
{
	// 세션 생성이 성공한 경우, 서버 여행을 통해 로비로 이동합니다.
	if (bWasSuccessful)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel(PathToLobby);
		}
	}
	else // 세션 생성이 실패한 경우, 화면에 오류 메시지를 출력하고 호스트 버튼을 다시 활성화합니다.
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Red,
				FString(TEXT("Failed to create session!"))
			);
		}
		HostButton->SetIsEnabled(true);
	}
}

// UMenu 클래스의 OnFindSessions 메서드 정의
// 세션 검색 결과를 처리하는 메서드입니다. 매치 타입이 일치하는 세션을 찾으면 해당 세션에 참여를 시도합니다.
void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	// 멀티플레이어 세션 서브시스템이 없다면 함수 실행을 종료합니다.
	if (MultiplayerSessionsSubsystem == nullptr)
	{
		return;
	}

	// 세션 결과를 순회하면서 설정된 매치 타입과 일치하는 세션을 찾아 참여를 시도합니다.
	for (auto Result : SessionResults)
	{
		FString SettingsValue;
		if (Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue) && SettingsValue == MatchType)
		{
			MultiplayerSessionsSubsystem->JoinSession(Result);
			return;
		}
	}
	// 세션 검색에 실패했거나 결과가 없는 경우, 조인 버튼을 다시 활성화합니다.
	if (!bWasSuccessful || SessionResults.Num() == 0)
	{
		JoinButton->SetIsEnabled(true);
	}
}


// 세션에 성공적으로 참여했을 때 실행되는 메서드입니다.
void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	// 온라인 서브시스템 인터페이스를 가져옵니다.
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		// 세션 인터페이스를 가져옵니다.
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			// 세션의 연결 문자열을 가져옵니다.
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

			// 게임 인스턴스로부터 첫 번째 로컬 플레이어 컨트롤러를 가져와 클라이언트 여행을 실행합니다.
			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (PlayerController)
			{
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
			}
		}
	}
}

// 세션 파괴가 완료되었을 때 호출되는 메서드입니다. 현재 비어있습니다.
void UMenu::OnDestroySession(bool bWasSuccessful)
{
}

// 세션 시작이 완료되었을 때 호출되는 메서드입니다. 현재 비어있습니다.
void UMenu::OnStartSession(bool bWasSuccessful)
{
}

// 호스트 버튼을 클릭했을 때 호출되는 메서드입니다. 새로운 세션을 생성하는 요청을 합니다.
void UMenu::HostButtonClicked()
{
	// 호스트 버튼을 비활성화합니다.
	HostButton->SetIsEnabled(false);
	if (MultiplayerSessionsSubsystem)
	{
		// 멀티플레이어 세션 서브시스템을 사용하여 새로운 세션을 생성합니다.
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
	}
}

// 조인 버튼을 클릭했을 때 호출되는 메서드입니다. 가능한 세션을 검색하는 요청을 합니다.
void UMenu::JoinButtonClicked()
{
	// 조인 버튼을 비활성화합니다.
	JoinButton->SetIsEnabled(false);
	if (MultiplayerSessionsSubsystem)
	{
		// 멀티플레이어 세션 서브시스템을 사용하여 세션을 검색합니다.
		MultiplayerSessionsSubsystem->FindSessions(10000);
	}
}

// 메뉴 UI를 해체할 때 호출되는 메서드입니다. UI를 제거하고 게임 모드의 입력 모드로 전환합니다.
void UMenu::MenuTearDown()
{
	// 메뉴 UI를 부모로부터 제거합니다.
	RemoveFromParent();
	// 현재 세계를 가져와서 플레이어 컨트롤러에게 게임 전용 입력 모드를 설정하고 마우스 커서를 숨깁니다.
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}


// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

// UMultiplayerSessionsSubsystem 클래스의 생성자 정의입니다.
UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem() :
	// 세션 관련 이벤트가 발생했을 때 호출될 콜백 메서드들을 대리자에 바인딩합니다.
	CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
	JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
	DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete)),
	StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete))
{
	// 온라인 서브시스템을 가져와서 세션 인터페이스를 설정합니다.
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		SessionInterface = Subsystem->GetSessionInterface();
	}
}

// 새로운 세션을 생성하는 메서드입니다.
void UMultiplayerSessionsSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType)
{
	// 요청된 공개 연결 수와 매치 타입을 저장합니다.
	DesiredNumPublicConnections = NumPublicConnections;
	DesiredMatchType = MatchType;

	// 세션 인터페이스 유효성을 검사합니다.
	if (!SessionInterface.IsValid())
	{
		return;
	}

	// 기존 세션을 검사하고 있으면 파괴 후 새로운 세션을 생성합니다.
	auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		bCreateSessionOnDestroy = true;
		LastNumPublicConnections = NumPublicConnections;
		LastMatchType = MatchType;

		DestroySession();
	}

	// 세션 생성 완료 대리자를 대리자 목록에 추가합니다.
	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	// 세션 설정을 초기화합니다.
	LastSessionSettings = MakeShareable(new FOnlineSessionSettings());
	LastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
	LastSessionSettings->NumPublicConnections = NumPublicConnections;
	LastSessionSettings->bAllowJoinInProgress = true;
	LastSessionSettings->bAllowJoinViaPresence = true;
	LastSessionSettings->bShouldAdvertise = true;
	LastSessionSettings->bUsesPresence = true;
	LastSessionSettings->Set(FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	LastSessionSettings->BuildUniqueId = 1;
	LastSessionSettings->bUseLobbiesIfAvailable = true;

	// 현재 게임의 첫 번째 로컬 플레이어를 가져옵니다.
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	// 세션 생성을 시도하고 실패하면, 우리의 커스텀 대리자를 통해 실패를 알립니다.
	if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
	{
		// 세션 생성 완료 대리자를 대리자 목록에서 제거합니다.
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);

		// 우리의 커스텀 대리자를 통해 세션 생성 실패를 방송합니다.
		MultiplayerOnCreateSessionComplete.Broadcast(false);
	}
}


// 멀티플레이어 세션을 찾기 위한 메서드입니다.
void UMultiplayerSessionsSubsystem::FindSessions(int32 MaxSearchResults)
{
	// 세션 인터페이스의 유효성을 검사합니다.
	if (!SessionInterface.IsValid())
	{
		return;
	}

	// 세션 검색 완료 이벤트를 처리할 대리자를 세션 인터페이스에 등록합니다.
	FindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	// 세션 검색 설정을 생성하고 초기화합니다.
	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	LastSessionSearch->MaxSearchResults = MaxSearchResults; // 최대 검색 결과 수를 설정합니다.
	LastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false; // LAN 검색 여부를 결정합니다.
	LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals); // 검색 쿼리 설정을 추가합니다.

	// 현재 게임의 첫 번째 로컬 플레이어를 가져옵니다.
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	// 세션 검색을 시작합니다. 실패한 경우, 대리자를 제거하고 실패를 방송합니다.
	if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
}

// 특정 세션에 참여하기 위한 메서드입니다.
void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
	// 세션 인터페이스의 유효성을 검사합니다.
	if (!SessionInterface.IsValid())
	{
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	// 세션 참여 완료 이벤트를 처리할 대리자를 세션 인터페이스에 등록합니다.
	JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

	// 현재 게임의 첫 번째 로컬 플레이어를 가져옵니다.
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	// 세션 참여를 시도합니다. 실패한 경우, 대리자를 제거하고 실패를 방송합니다.
	if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult))
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}

// 현재 세션을 파괴하기 위한 메서드입니다.
void UMultiplayerSessionsSubsystem::DestroySession()
{
	// 세션 인터페이스의 유효성을 검사합니다.
	if (!SessionInterface.IsValid())
	{
		MultiplayerOnDestroySessionComplete.Broadcast(false);
		return;
	}

	// 세션 파괴 완료 이벤트를 처리할 대리자를 세션 인터페이스에 등록합니다.
	DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);

	// 세션 파괴를 시도합니다. 실패한 경우, 대리자를 제거하고 실패를 방송합니다.
	if (!SessionInterface->DestroySession(NAME_GameSession))
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
		MultiplayerOnDestroySessionComplete.Broadcast(false);
	}
}

// 현재 세션을 시작하기 위한 메서드입니다. 현재 구현되지 않았습니다.
void UMultiplayerSessionsSubsystem::StartSession()
{
	
}


// 세션 생성이 완료된 후 호출되는 콜백 메서드입니다.
void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	// 세션 인터페이스가 유효하면, 세션 생성 완료 대리자를 대리자 목록에서 제거합니다.
	if (SessionInterface)
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	// 세션 생성 결과를 방송합니다.
	MultiplayerOnCreateSessionComplete.Broadcast(bWasSuccessful);
}

// 세션 검색이 완료된 후 호출되는 콜백 메서드입니다.
void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	// 세션 인터페이스가 유효하면, 세션 검색 완료 대리자를 대리자 목록에서 제거합니다.
	if (SessionInterface)
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}

	// 검색 결과가 없다면, 실패한 검색 결과를 방송합니다.
	if (LastSessionSearch->SearchResults.Num() <= 0)
	{
		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	// 세션 검색 결과를 방송합니다.
	MultiplayerOnFindSessionsComplete.Broadcast(LastSessionSearch->SearchResults, bWasSuccessful);
}

// 세션 참여가 완료된 후 호출되는 콜백 메서드입니다.
void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	// 세션 인터페이스가 유효하면, 세션 참여 완료 대리자를 대리자 목록에서 제거합니다.
	if (SessionInterface)
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}

	// 세션 참여 결과를 방송합니다.
	MultiplayerOnJoinSessionComplete.Broadcast(Result);
}

// 세션 파괴가 완료된 후 호출되는 콜백 메서드입니다.
void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	// 세션 인터페이스가 유효하면, 세션 파괴 완료 대리자를 대리자 목록에서 제거합니다.
	if (SessionInterface)
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}

	// 세션 파괴가 성공적이고, 세션 파괴 후 새 세션 생성 플래그가 설정된 경우, 새 세션을 생성합니다.
	if (bWasSuccessful && bCreateSessionOnDestroy)
	{
		bCreateSessionOnDestroy = false;
		CreateSession(LastNumPublicConnections, LastMatchType);
	}

	// 세션 파괴 결과를 방송합니다.
	MultiplayerOnDestroySessionComplete.Broadcast(bWasSuccessful);
}

// 세션 시작이 완료된 후 호출되는 콜백 메서드입니다. 현재 구현되지 않았습니다.
void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
	
}


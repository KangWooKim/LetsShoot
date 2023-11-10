// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

// UMultiplayerSessionsSubsystem Ŭ������ ������ �����Դϴ�.
UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem() :
	// ���� ���� �̺�Ʈ�� �߻����� �� ȣ��� �ݹ� �޼������ �븮�ڿ� ���ε��մϴ�.
	CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete)),
	FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete)),
	JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete)),
	DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete)),
	StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete))
{
	// �¶��� ����ý����� �����ͼ� ���� �������̽��� �����մϴ�.
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		SessionInterface = Subsystem->GetSessionInterface();
	}
}

// ���ο� ������ �����ϴ� �޼����Դϴ�.
void UMultiplayerSessionsSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType)
{
	// ��û�� ���� ���� ���� ��ġ Ÿ���� �����մϴ�.
	DesiredNumPublicConnections = NumPublicConnections;
	DesiredMatchType = MatchType;

	// ���� �������̽� ��ȿ���� �˻��մϴ�.
	if (!SessionInterface.IsValid())
	{
		return;
	}

	// ���� ������ �˻��ϰ� ������ �ı� �� ���ο� ������ �����մϴ�.
	auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession);
	if (ExistingSession != nullptr)
	{
		bCreateSessionOnDestroy = true;
		LastNumPublicConnections = NumPublicConnections;
		LastMatchType = MatchType;

		DestroySession();
	}

	// ���� ���� �Ϸ� �븮�ڸ� �븮�� ��Ͽ� �߰��մϴ�.
	CreateSessionCompleteDelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	// ���� ������ �ʱ�ȭ�մϴ�.
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

	// ���� ������ ù ��° ���� �÷��̾ �����ɴϴ�.
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	// ���� ������ �õ��ϰ� �����ϸ�, �츮�� Ŀ���� �븮�ڸ� ���� ���и� �˸��ϴ�.
	if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
	{
		// ���� ���� �Ϸ� �븮�ڸ� �븮�� ��Ͽ��� �����մϴ�.
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);

		// �츮�� Ŀ���� �븮�ڸ� ���� ���� ���� ���и� ����մϴ�.
		MultiplayerOnCreateSessionComplete.Broadcast(false);
	}
}


// ��Ƽ�÷��̾� ������ ã�� ���� �޼����Դϴ�.
void UMultiplayerSessionsSubsystem::FindSessions(int32 MaxSearchResults)
{
	// ���� �������̽��� ��ȿ���� �˻��մϴ�.
	if (!SessionInterface.IsValid())
	{
		return;
	}

	// ���� �˻� �Ϸ� �̺�Ʈ�� ó���� �븮�ڸ� ���� �������̽��� ����մϴ�.
	FindSessionsCompleteDelegateHandle = SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	// ���� �˻� ������ �����ϰ� �ʱ�ȭ�մϴ�.
	LastSessionSearch = MakeShareable(new FOnlineSessionSearch());
	LastSessionSearch->MaxSearchResults = MaxSearchResults; // �ִ� �˻� ��� ���� �����մϴ�.
	LastSessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false; // LAN �˻� ���θ� �����մϴ�.
	LastSessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals); // �˻� ���� ������ �߰��մϴ�.

	// ���� ������ ù ��° ���� �÷��̾ �����ɴϴ�.
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	// ���� �˻��� �����մϴ�. ������ ���, �븮�ڸ� �����ϰ� ���и� ����մϴ�.
	if (!SessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), LastSessionSearch.ToSharedRef()))
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
}

// Ư�� ���ǿ� �����ϱ� ���� �޼����Դϴ�.
void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
	// ���� �������̽��� ��ȿ���� �˻��մϴ�.
	if (!SessionInterface.IsValid())
	{
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
		return;
	}

	// ���� ���� �Ϸ� �̺�Ʈ�� ó���� �븮�ڸ� ���� �������̽��� ����մϴ�.
	JoinSessionCompleteDelegateHandle = SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

	// ���� ������ ù ��° ���� �÷��̾ �����ɴϴ�.
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	// ���� ������ �õ��մϴ�. ������ ���, �븮�ڸ� �����ϰ� ���и� ����մϴ�.
	if (!SessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SessionResult))
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
		MultiplayerOnJoinSessionComplete.Broadcast(EOnJoinSessionCompleteResult::UnknownError);
	}
}

// ���� ������ �ı��ϱ� ���� �޼����Դϴ�.
void UMultiplayerSessionsSubsystem::DestroySession()
{
	// ���� �������̽��� ��ȿ���� �˻��մϴ�.
	if (!SessionInterface.IsValid())
	{
		MultiplayerOnDestroySessionComplete.Broadcast(false);
		return;
	}

	// ���� �ı� �Ϸ� �̺�Ʈ�� ó���� �븮�ڸ� ���� �������̽��� ����մϴ�.
	DestroySessionCompleteDelegateHandle = SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);

	// ���� �ı��� �õ��մϴ�. ������ ���, �븮�ڸ� �����ϰ� ���и� ����մϴ�.
	if (!SessionInterface->DestroySession(NAME_GameSession))
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
		MultiplayerOnDestroySessionComplete.Broadcast(false);
	}
}

// ���� ������ �����ϱ� ���� �޼����Դϴ�. ���� �������� �ʾҽ��ϴ�.
void UMultiplayerSessionsSubsystem::StartSession()
{
	
}


// ���� ������ �Ϸ�� �� ȣ��Ǵ� �ݹ� �޼����Դϴ�.
void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	// ���� �������̽��� ��ȿ�ϸ�, ���� ���� �Ϸ� �븮�ڸ� �븮�� ��Ͽ��� �����մϴ�.
	if (SessionInterface)
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
	}

	// ���� ���� ����� ����մϴ�.
	MultiplayerOnCreateSessionComplete.Broadcast(bWasSuccessful);
}

// ���� �˻��� �Ϸ�� �� ȣ��Ǵ� �ݹ� �޼����Դϴ�.
void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	// ���� �������̽��� ��ȿ�ϸ�, ���� �˻� �Ϸ� �븮�ڸ� �븮�� ��Ͽ��� �����մϴ�.
	if (SessionInterface)
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegateHandle);
	}

	// �˻� ����� ���ٸ�, ������ �˻� ����� ����մϴ�.
	if (LastSessionSearch->SearchResults.Num() <= 0)
	{
		MultiplayerOnFindSessionsComplete.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		return;
	}

	// ���� �˻� ����� ����մϴ�.
	MultiplayerOnFindSessionsComplete.Broadcast(LastSessionSearch->SearchResults, bWasSuccessful);
}

// ���� ������ �Ϸ�� �� ȣ��Ǵ� �ݹ� �޼����Դϴ�.
void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	// ���� �������̽��� ��ȿ�ϸ�, ���� ���� �Ϸ� �븮�ڸ� �븮�� ��Ͽ��� �����մϴ�.
	if (SessionInterface)
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);
	}

	// ���� ���� ����� ����մϴ�.
	MultiplayerOnJoinSessionComplete.Broadcast(Result);
}

// ���� �ı��� �Ϸ�� �� ȣ��Ǵ� �ݹ� �޼����Դϴ�.
void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	// ���� �������̽��� ��ȿ�ϸ�, ���� �ı� �Ϸ� �븮�ڸ� �븮�� ��Ͽ��� �����մϴ�.
	if (SessionInterface)
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegateHandle);
	}

	// ���� �ı��� �������̰�, ���� �ı� �� �� ���� ���� �÷��װ� ������ ���, �� ������ �����մϴ�.
	if (bWasSuccessful && bCreateSessionOnDestroy)
	{
		bCreateSessionOnDestroy = false;
		CreateSession(LastNumPublicConnections, LastMatchType);
	}

	// ���� �ı� ����� ����մϴ�.
	MultiplayerOnDestroySessionComplete.Broadcast(bWasSuccessful);
}

// ���� ������ �Ϸ�� �� ȣ��Ǵ� �ݹ� �޼����Դϴ�. ���� �������� �ʾҽ��ϴ�.
void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
	
}


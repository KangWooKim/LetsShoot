// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

// UMenu Ŭ������ MenuSetup �޼��� ����
// �� �޼���� �޴� UI�� �����ϰ�, ��Ƽ�÷��̾� ���� ����ý����� �ʱ�ȭ�մϴ�.
void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath)
{
	// �κ���� ��θ� �����մϴ�. 'listen' ���� �Ķ���͸� �߰��ؼ� ������ ��� ���°� �ǵ��� �մϴ�.
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);

	// ���� ���� ���� ��ġ Ÿ���� �����մϴ�.
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;

	// ����Ʈ�� �޴��� �߰��ϰ�, ���ü��� �����մϴ�.
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	// ���� ���踦 ������ �÷��̾� ��Ʈ�ѷ��� UI ���� �Է� ��带 �����մϴ�.
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

	// ���� �ν��Ͻ����� ��Ƽ�÷��̾� ���� ����ý����� �����ɴϴ�.
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	// ��Ƽ�÷��̾� ���� ����ý����� ������, ���� ���� �ݹ��� �����մϴ�.
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
		MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
	}
}

// UMenu Ŭ������ Initialize �޼��� ����
// �� �޼���� UI ��ҵ��� ����� �ʱ�ȭ�Ǿ����� Ȯ���ϰ�, �� ��ư�� ������ �ݹ��� �����մϴ�.
bool UMenu::Initialize()
{
	// �θ� Ŭ������ Initialize �޼��带 ȣ���Ͽ� �⺻ �ʱ�ȭ�� �����մϴ�.
	if (!Super::Initialize())
	{
		return false;
	}

	// ȣ��Ʈ ��ư�� ������, Ŭ�� �̺�Ʈ�� HostButtonClicked �޼��带 �����մϴ�.
	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}
	// ���� ��ư�� ������, Ŭ�� �̺�Ʈ�� JoinButtonClicked �޼��带 �����մϴ�.
	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}

	// �ʱ�ȭ�� ���������� �Ϸ�Ǹ� true�� ��ȯ�մϴ�.
	return true;
}


// UMenu Ŭ������ NativeDestruct �޼��� ����
// �� �޼���� UObject�� �ı��� �� ȣ��Ǹ�, MenuTearDown�� ȣ���� �� �⺻ �ı��ڸ� ȣ���մϴ�.
void UMenu::NativeDestruct()
{
	// �޴��� ��ü�ϴ� �۾��� �����մϴ�.
	MenuTearDown();

	// �⺻ Ŭ������ �ı��ڸ� ȣ���մϴ�.
	Super::NativeDestruct();
}

// UMenu Ŭ������ OnCreateSession �޼��� ����
// ���� ���� ��û�� ����� ó���ϴ� �޼����Դϴ�. �����ϸ� �κ�� �̵��ϰ�, �����ϸ� ���� �޽����� ǥ���մϴ�.
void UMenu::OnCreateSession(bool bWasSuccessful)
{
	// ���� ������ ������ ���, ���� ������ ���� �κ�� �̵��մϴ�.
	if (bWasSuccessful)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel(PathToLobby);
		}
	}
	else // ���� ������ ������ ���, ȭ�鿡 ���� �޽����� ����ϰ� ȣ��Ʈ ��ư�� �ٽ� Ȱ��ȭ�մϴ�.
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

// UMenu Ŭ������ OnFindSessions �޼��� ����
// ���� �˻� ����� ó���ϴ� �޼����Դϴ�. ��ġ Ÿ���� ��ġ�ϴ� ������ ã���� �ش� ���ǿ� ������ �õ��մϴ�.
void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	// ��Ƽ�÷��̾� ���� ����ý����� ���ٸ� �Լ� ������ �����մϴ�.
	if (MultiplayerSessionsSubsystem == nullptr)
	{
		return;
	}

	// ���� ����� ��ȸ�ϸ鼭 ������ ��ġ Ÿ�԰� ��ġ�ϴ� ������ ã�� ������ �õ��մϴ�.
	for (auto Result : SessionResults)
	{
		FString SettingsValue;
		if (Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue) && SettingsValue == MatchType)
		{
			MultiplayerSessionsSubsystem->JoinSession(Result);
			return;
		}
	}
	// ���� �˻��� �����߰ų� ����� ���� ���, ���� ��ư�� �ٽ� Ȱ��ȭ�մϴ�.
	if (!bWasSuccessful || SessionResults.Num() == 0)
	{
		JoinButton->SetIsEnabled(true);
	}
}


// ���ǿ� ���������� �������� �� ����Ǵ� �޼����Դϴ�.
void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	// �¶��� ����ý��� �������̽��� �����ɴϴ�.
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		// ���� �������̽��� �����ɴϴ�.
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			// ������ ���� ���ڿ��� �����ɴϴ�.
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

			// ���� �ν��Ͻ��κ��� ù ��° ���� �÷��̾� ��Ʈ�ѷ��� ������ Ŭ���̾�Ʈ ������ �����մϴ�.
			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (PlayerController)
			{
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
			}
		}
	}
}

// ���� �ı��� �Ϸ�Ǿ��� �� ȣ��Ǵ� �޼����Դϴ�. ���� ����ֽ��ϴ�.
void UMenu::OnDestroySession(bool bWasSuccessful)
{
}

// ���� ������ �Ϸ�Ǿ��� �� ȣ��Ǵ� �޼����Դϴ�. ���� ����ֽ��ϴ�.
void UMenu::OnStartSession(bool bWasSuccessful)
{
}

// ȣ��Ʈ ��ư�� Ŭ������ �� ȣ��Ǵ� �޼����Դϴ�. ���ο� ������ �����ϴ� ��û�� �մϴ�.
void UMenu::HostButtonClicked()
{
	// ȣ��Ʈ ��ư�� ��Ȱ��ȭ�մϴ�.
	HostButton->SetIsEnabled(false);
	if (MultiplayerSessionsSubsystem)
	{
		// ��Ƽ�÷��̾� ���� ����ý����� ����Ͽ� ���ο� ������ �����մϴ�.
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
	}
}

// ���� ��ư�� Ŭ������ �� ȣ��Ǵ� �޼����Դϴ�. ������ ������ �˻��ϴ� ��û�� �մϴ�.
void UMenu::JoinButtonClicked()
{
	// ���� ��ư�� ��Ȱ��ȭ�մϴ�.
	JoinButton->SetIsEnabled(false);
	if (MultiplayerSessionsSubsystem)
	{
		// ��Ƽ�÷��̾� ���� ����ý����� ����Ͽ� ������ �˻��մϴ�.
		MultiplayerSessionsSubsystem->FindSessions(10000);
	}
}

// �޴� UI�� ��ü�� �� ȣ��Ǵ� �޼����Դϴ�. UI�� �����ϰ� ���� ����� �Է� ���� ��ȯ�մϴ�.
void UMenu::MenuTearDown()
{
	// �޴� UI�� �θ�κ��� �����մϴ�.
	RemoveFromParent();
	// ���� ���踦 �����ͼ� �÷��̾� ��Ʈ�ѷ����� ���� ���� �Է� ��带 �����ϰ� ���콺 Ŀ���� ����ϴ�.
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


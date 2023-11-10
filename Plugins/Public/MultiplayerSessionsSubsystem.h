// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "MultiplayerSessionsSubsystem.generated.h"

//
// Delcaring our own custom delegates for the Menu class to bind callbacks to
//
// ���� ������ �Ϸ�Ǿ��� �� ȣ��� �� �ִ� ���̳��� ��Ƽĳ��Ʈ �븮�ڸ� �����մϴ�.
// �� �븮�ڴ� ���� ������ ���� ���θ� �Ҹ��� ������ �޽��ϴ�.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnCreateSessionComplete, bool, bWasSuccessful);

// ���� �˻��� �Ϸ�Ǿ��� �� ȣ��� ��Ƽĳ��Ʈ �븮�ڸ� �����մϴ�.
// �� �븮�ڴ� ã�Ƴ� ���ǵ��� �迭�� �˻��� ���� ���θ� �Ҹ��� ������ �޽��ϴ�.
DECLARE_MULTICAST_DELEGATE_TwoParams(FMultiplayerOnFindSessionsComplete, const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);

// ���� ������ �Ϸ�Ǿ��� �� ȣ��� ��Ƽĳ��Ʈ �븮�ڸ� �����մϴ�.
// �� �븮�ڴ� ���� ����� EOnJoinSessionCompleteResult::Type ���������� �޽��ϴ�.
DECLARE_MULTICAST_DELEGATE_OneParam(FMultiplayerOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type Result);

// ���� �ı��� �Ϸ�Ǿ��� �� ȣ��� �� �ִ� ���̳��� ��Ƽĳ��Ʈ �븮�ڸ� �����մϴ�.
// �� �븮�ڴ� ���� �ı��� ���� ���θ� �Ҹ��� ������ �޽��ϴ�.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnDestroySessionComplete, bool, bWasSuccessful);

// ���� ������ �Ϸ�Ǿ��� �� ȣ��� �� �ִ� ���̳��� ��Ƽĳ��Ʈ �븮�ڸ� �����մϴ�.
// �� �븮�ڴ� ���� ������ ���� ���θ� �Ҹ��� ������ �޽��ϴ�.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnStartSessionComplete, bool, bWasSuccessful);

/**
 * 
 */
UCLASS()
// ��Ƽ�÷��̾� ���� ����� ó���ϴ� ����ý��� Ŭ�����Դϴ�.
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    // �⺻ ������
    UMultiplayerSessionsSubsystem();

    // �޴� Ŭ������ ȣ���� ���� ��� ó�� �޼�����Դϴ�.
    void CreateSession(int32 NumPublicConnections, FString MatchType); // ���ο� ������ �����մϴ�.
    void FindSessions(int32 MaxSearchResults); // ������ ���ǵ��� �˻��մϴ�.
    void JoinSession(const FOnlineSessionSearchResult& SessionResult); // Ư�� ���ǿ� ������ �õ��մϴ�.
    void DestroySession(); // ���� ������ �ı��մϴ�.
    void StartSession(); // ������ �����մϴ�.

    // �޴� Ŭ������ �ݹ��� ���ε��� �� �ִ� Ŀ���� �븮�ڵ��Դϴ�.
    FMultiplayerOnCreateSessionComplete MultiplayerOnCreateSessionComplete;
    FMultiplayerOnFindSessionsComplete MultiplayerOnFindSessionsComplete;
    FMultiplayerOnJoinSessionComplete MultiplayerOnJoinSessionComplete;
    FMultiplayerOnDestroySessionComplete MultiplayerOnDestroySessionComplete;
    FMultiplayerOnStartSessionComplete MultiplayerOnStartSessionComplete;

    // ���ϴ� ���� ���� ���� ��ġ Ÿ���� �����մϴ�.
    int32 DesiredNumPublicConnections{};
    FString DesiredMatchType{};
protected:

    // �¶��� ���� �������̽��� �븮�� ��Ͽ� �߰��� ���� �ݹ� �޼�����Դϴ�.
    // �� Ŭ���� �ܺο��� ȣ���� �ʿ䰡 �����ϴ�.
    void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
    void OnFindSessionsComplete(bool bWasSuccessful);
    void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
    void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
    void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);

private:
    // �¶��� ���� �������̽� ������
    IOnlineSessionPtr SessionInterface;
    // ������ ���� ����
    TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
    // ������ ���� �˻�
    TSharedPtr<FOnlineSessionSearch> LastSessionSearch;

    // �¶��� ���� �������̽� �븮�� ��Ͽ� �߰��� �븮�ڵ��Դϴ�.
    // �̵��� MultiplayerSessionsSubsystem ���� �ݹ鿡 ���ε��˴ϴ�.
    FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
    FDelegateHandle CreateSessionCompleteDelegateHandle;
    FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
    FDelegateHandle FindSessionsCompleteDelegateHandle;
    FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
    FDelegateHandle JoinSessionCompleteDelegateHandle;
    FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
    FDelegateHandle DestroySessionCompleteDelegateHandle;
    FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
    FDelegateHandle StartSessionCompleteDelegateHandle;

    // ���� �ı� �� ���ο� ������ ������ �� ����
    bool bCreateSessionOnDestroy{ false };
    // ���������� ��û�� ���� ���� ��
    int32 LastNumPublicConnections;
    // ���������� ��û�� ��ġ Ÿ��
    FString LastMatchType;
};


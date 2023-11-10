// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "MultiplayerSessionsSubsystem.generated.h"

//
// Delcaring our own custom delegates for the Menu class to bind callbacks to
//
// 세션 생성이 완료되었을 때 호출될 수 있는 다이나믹 멀티캐스트 대리자를 선언합니다.
// 이 대리자는 세션 생성의 성공 여부를 불리언 값으로 받습니다.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnCreateSessionComplete, bool, bWasSuccessful);

// 세션 검색이 완료되었을 때 호출될 멀티캐스트 대리자를 선언합니다.
// 이 대리자는 찾아낸 세션들의 배열과 검색의 성공 여부를 불리언 값으로 받습니다.
DECLARE_MULTICAST_DELEGATE_TwoParams(FMultiplayerOnFindSessionsComplete, const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);

// 세션 참여가 완료되었을 때 호출될 멀티캐스트 대리자를 선언합니다.
// 이 대리자는 참여 결과를 EOnJoinSessionCompleteResult::Type 열거형으로 받습니다.
DECLARE_MULTICAST_DELEGATE_OneParam(FMultiplayerOnJoinSessionComplete, EOnJoinSessionCompleteResult::Type Result);

// 세션 파괴가 완료되었을 때 호출될 수 있는 다이나믹 멀티캐스트 대리자를 선언합니다.
// 이 대리자는 세션 파괴의 성공 여부를 불리언 값으로 받습니다.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnDestroySessionComplete, bool, bWasSuccessful);

// 세션 시작이 완료되었을 때 호출될 수 있는 다이나믹 멀티캐스트 대리자를 선언합니다.
// 이 대리자는 세션 시작의 성공 여부를 불리언 값으로 받습니다.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiplayerOnStartSessionComplete, bool, bWasSuccessful);

/**
 * 
 */
UCLASS()
// 멀티플레이어 세션 기능을 처리하는 서브시스템 클래스입니다.
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()
public:
    // 기본 생성자
    UMultiplayerSessionsSubsystem();

    // 메뉴 클래스가 호출할 세션 기능 처리 메서드들입니다.
    void CreateSession(int32 NumPublicConnections, FString MatchType); // 새로운 세션을 생성합니다.
    void FindSessions(int32 MaxSearchResults); // 가능한 세션들을 검색합니다.
    void JoinSession(const FOnlineSessionSearchResult& SessionResult); // 특정 세션에 참여를 시도합니다.
    void DestroySession(); // 현재 세션을 파괴합니다.
    void StartSession(); // 세션을 시작합니다.

    // 메뉴 클래스가 콜백을 바인딩할 수 있는 커스텀 대리자들입니다.
    FMultiplayerOnCreateSessionComplete MultiplayerOnCreateSessionComplete;
    FMultiplayerOnFindSessionsComplete MultiplayerOnFindSessionsComplete;
    FMultiplayerOnJoinSessionComplete MultiplayerOnJoinSessionComplete;
    FMultiplayerOnDestroySessionComplete MultiplayerOnDestroySessionComplete;
    FMultiplayerOnStartSessionComplete MultiplayerOnStartSessionComplete;

    // 원하는 공개 연결 수와 매치 타입을 저장합니다.
    int32 DesiredNumPublicConnections{};
    FString DesiredMatchType{};
protected:

    // 온라인 세션 인터페이스의 대리자 목록에 추가할 내부 콜백 메서드들입니다.
    // 이 클래스 외부에서 호출할 필요가 없습니다.
    void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
    void OnFindSessionsComplete(bool bWasSuccessful);
    void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
    void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
    void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);

private:
    // 온라인 세션 인터페이스 포인터
    IOnlineSessionPtr SessionInterface;
    // 마지막 세션 설정
    TSharedPtr<FOnlineSessionSettings> LastSessionSettings;
    // 마지막 세션 검색
    TSharedPtr<FOnlineSessionSearch> LastSessionSearch;

    // 온라인 세션 인터페이스 대리자 목록에 추가할 대리자들입니다.
    // 이들은 MultiplayerSessionsSubsystem 내부 콜백에 바인딩됩니다.
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

    // 세션 파괴 후 새로운 세션을 생성할 지 여부
    bool bCreateSessionOnDestroy{ false };
    // 마지막으로 요청된 공개 연결 수
    int32 LastNumPublicConnections;
    // 마지막으로 요청된 매치 타입
    FString LastMatchType;
};


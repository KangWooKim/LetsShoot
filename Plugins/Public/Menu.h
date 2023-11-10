#pragma once // 한 번만 포함되도록 하는 전처리기 지시문입니다.

#include "CoreMinimal.h" 
#include "Blueprint/UserWidget.h" // 사용자 위젯을 위한 블루프린트 기반 클래스를 포함합니다.
#include "Interfaces/OnlineSessionInterface.h" // 온라인 세션 인터페이스를 포함합니다.
#include "Menu.generated.h" 

UCLASS() 
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget // UMenu 클래스 선언, UUserWidget에서 상속받음.
{
	GENERATED_BODY() 

public:
	UFUNCTION(BlueprintCallable) // 블루프린트에서 호출할 수 있는 함수임을 나타냅니다.
		void MenuSetup(int32 NumberOfPublicConnections = 4, FString TypeOfMatch = FString(TEXT("FreeForAll")), FString LobbyPath = FString(TEXT("/Game/ThirdPersonCPP/Maps/Lobby"))); // 메뉴 설정 함수.

protected:
	virtual bool Initialize() override; // 위젯 초기화 함수.
	virtual void NativeDestruct() override; // 위젯 소멸 시 호출되는 함수.

	// 멀티플레이어 세션 서브시스템의 사용자 정의 대리자들을 위한 콜백 함수들.
	UFUNCTION()
		void OnCreateSession(bool bWasSuccessful);
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
		void OnDestroySession(bool bWasSuccessful);
	UFUNCTION()
		void OnStartSession(bool bWasSuccessful);

private:
	UPROPERTY(meta = (BindWidget))
		class UButton* HostButton; // 호스트 버튼 위젯에 대한 포인터.

	UPROPERTY(meta = (BindWidget))
		UButton* JoinButton; // 참여 버튼 위젯에 대한 포인터.

	UFUNCTION()
		void HostButtonClicked(); // 호스트 버튼 클릭 시 호출되는 함수.

	UFUNCTION()
		void JoinButtonClicked(); // 참여 버튼 클릭 시 호출되는 함수.

	void MenuTearDown(); // 메뉴 해체 함수.

	// 온라인 세션 기능을 처리하는 서브시스템.
	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		int32 NumPublicConnections {
		4
	}; // 공개 연결 수.

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		FString MatchType {
		TEXT("FreeForAll")
	}; // 매치 타입.

	FString PathToLobby{ TEXT("") }; // 로비로의 경로.
};

#pragma once // �� ���� ���Եǵ��� �ϴ� ��ó���� ���ù��Դϴ�.

#include "CoreMinimal.h" 
#include "Blueprint/UserWidget.h" // ����� ������ ���� �������Ʈ ��� Ŭ������ �����մϴ�.
#include "Interfaces/OnlineSessionInterface.h" // �¶��� ���� �������̽��� �����մϴ�.
#include "Menu.generated.h" 

UCLASS() 
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget // UMenu Ŭ���� ����, UUserWidget���� ��ӹ���.
{
	GENERATED_BODY() 

public:
	UFUNCTION(BlueprintCallable) // �������Ʈ���� ȣ���� �� �ִ� �Լ����� ��Ÿ���ϴ�.
		void MenuSetup(int32 NumberOfPublicConnections = 4, FString TypeOfMatch = FString(TEXT("FreeForAll")), FString LobbyPath = FString(TEXT("/Game/ThirdPersonCPP/Maps/Lobby"))); // �޴� ���� �Լ�.

protected:
	virtual bool Initialize() override; // ���� �ʱ�ȭ �Լ�.
	virtual void NativeDestruct() override; // ���� �Ҹ� �� ȣ��Ǵ� �Լ�.

	// ��Ƽ�÷��̾� ���� ����ý����� ����� ���� �븮�ڵ��� ���� �ݹ� �Լ���.
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
		class UButton* HostButton; // ȣ��Ʈ ��ư ������ ���� ������.

	UPROPERTY(meta = (BindWidget))
		UButton* JoinButton; // ���� ��ư ������ ���� ������.

	UFUNCTION()
		void HostButtonClicked(); // ȣ��Ʈ ��ư Ŭ�� �� ȣ��Ǵ� �Լ�.

	UFUNCTION()
		void JoinButtonClicked(); // ���� ��ư Ŭ�� �� ȣ��Ǵ� �Լ�.

	void MenuTearDown(); // �޴� ��ü �Լ�.

	// �¶��� ���� ����� ó���ϴ� ����ý���.
	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		int32 NumPublicConnections {
		4
	}; // ���� ���� ��.

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		FString MatchType {
		TEXT("FreeForAll")
	}; // ��ġ Ÿ��.

	FString PathToLobby{ TEXT("") }; // �κ���� ���.
};

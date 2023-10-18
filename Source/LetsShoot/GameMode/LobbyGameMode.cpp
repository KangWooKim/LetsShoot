// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "MultiplayerSessionsSubsystem.h"
// 새로운 플레이어가 게임에 로그인했을 때 호출되는 함수
void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	// 부모 클래스의 PostLogin 함수 호출
	Super::PostLogin(NewPlayer);

	// 현재 게임 상태에 접속한 플레이어의 수를 가져옵니다.
	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();

	// 현재 게임 인스턴스를 가져옵니다.
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		// 멀티플레이어 세션 서브시스템을 가져옵니다.
		UMultiplayerSessionsSubsystem* Subsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
		// 서브시스템이 유효한지 체크합니다.
		check(Subsystem);

		// 현재 플레이어 수가 설정된 원하는 연결 수와 같은지 확인합니다.
		if (NumberOfPlayers == Subsystem->DesiredNumPublicConnections)
		{
			// 월드 인스턴스를 가져옵니다.
			UWorld* World = GetWorld();
			if (World)
			{
				// 월드에서 심리스 트래블을 사용하도록 설정합니다.
				bUseSeamlessTravel = true;

				// 원하는 매치 타입을 가져옵니다.
				FString MatchType = Subsystem->DesiredMatchType;
				// 매치 타입에 따라 서버 트래블을 수행합니다.
				if (MatchType == "FreeForAll")
				{
					// 자유롭게 모두와 싸우는 맵으로 이동합니다.
					World->ServerTravel(FString("/Game/Maps/BlasterMap?listen"));
				}
				else if (MatchType == "Teams")
				{
					// 팀을 구성하여 싸우는 맵으로 이동합니다.
					World->ServerTravel(FString("/Game/Maps/Teams?listen"));
				}
				else if (MatchType == "CaptureTheFlag")
				{
					// 깃발 잡기 모드의 맵으로 이동합니다.
					World->ServerTravel(FString("/Game/Maps/CaptureTheFlag?listen"));
				}
			}
		}
	}
}

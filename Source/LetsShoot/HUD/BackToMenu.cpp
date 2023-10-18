// Fill out your copyright notice in the Description page of Project Settings.


#include "BackToMenu.h"
#include "GameFramework/PlayerController.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "GameFramework/GameModeBase.h"
#include "LetsShoot/Character/LetsShootCharacter.h"

// UBackToMenu 클래스를 정의합니다.

// MenuSetup 메서드
// 이 메서드는 메뉴 설정을 담당합니다.
void UBackToMenu::MenuSetup()
{
    // 메뉴를 뷰포트에 추가하고, 메뉴를 보이게 설정하며, 포커스를 받을 수 있도록 설정합니다.
    AddToViewport();
    SetVisibility(ESlateVisibility::Visible);
    bIsFocusable = true;

    // 게임의 세계를 가져옵니다.
    UWorld* World = GetWorld();
    if (World)
    {
        // PlayerController가 null이면 첫 번째 플레이어 컨트롤러를 가져옵니다.
        PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
        if (PlayerController)
        {
            // 입력 모드를 설정하고, 마우스 커서를 보이게 합니다.
            FInputModeGameAndUI InputModeData;
            InputModeData.SetWidgetToFocus(TakeWidget());
            PlayerController->SetInputMode(InputModeData);
            PlayerController->SetShowMouseCursor(true);
        }
    }
    // ReturnButton이 바인딩되어 있지 않으면 클릭 이벤트를 추가합니다.
    if (ReturnButton && !ReturnButton->OnClicked.IsBound())
    {
        ReturnButton->OnClicked.AddDynamic(this, &UBackToMenu::ReturnButtonClicked);
    }
    // 게임 인스턴스를 가져와서 멀티플레이어 세션 서브시스템을 가져옵니다.
    UGameInstance* GameInstance = GetGameInstance();
    if (GameInstance)
    {
        MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
        if (MultiplayerSessionsSubsystem)
        {
            // 세션 파괴 완료 이벤트에 리스너를 추가합니다.
            MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &UBackToMenu::OnDestroySession);
        }
    }
}

// Initialize 메서드
// 이 메서드는 초기화를 담당합니다.
bool UBackToMenu::Initialize()
{
    // 상위 클래스의 Initialize 메서드를 호출하여 초기화를 시도합니다.
    if (!Super::Initialize())
    {
        // 초기화가 실패하면 false를 반환합니다.
        return false;
    }

    // 초기화가 성공하면 true를 반환합니다.
    return true;
}

// OnDestroySession 메서드
// 이 메서드는 세션 파괴 이벤트를 처리합니다.
// 매개변수:
// - bWasSuccessful: 세션 파괴가 성공적으로 이루어졌는지 나타내는 bool 값입니다.
void UBackToMenu::OnDestroySession(bool bWasSuccessful)
{
    // 세션 파괴가 실패하면 ReturnButton을 활성화하고 메서드를 종료합니다.
    if (!bWasSuccessful)
    {
        ReturnButton->SetIsEnabled(true);
        return;
    }

    // 게임의 세계를 가져옵니다.
    UWorld* World = GetWorld();
    if (World)
    {
        // 게임 모드를 가져옵니다.
        AGameModeBase* GameMode = World->GetAuthGameMode<AGameModeBase>();
        if (GameMode)
        {
            // 게임 모드가 있으면 메인 메뉴로 돌아갑니다.
            GameMode->BackToMenuHost();
        }
        else
        {
            // PlayerController가 null이면 첫 번째 플레이어 컨트롤러를 가져옵니다.
            PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
            if (PlayerController)
            {
                // PlayerController가 있으면 클라이언트를 메인 메뉴로 돌려보냅니다.
                PlayerController->ClientBackToMenuWithTextReason(FText());
            }
        }
    }
}


// MenuTearDown 메서드
// 이 메서드는 메뉴를 해제하는 역할을 합니다.
void UBackToMenu::MenuTearDown()
{
    // 부모 위젯으로부터 현재 위젯을 제거합니다.
    RemoveFromParent();

    // 게임의 세계를 가져옵니다.
    UWorld* World = GetWorld();
    if (World)
    {
        // PlayerController가 null이면 첫 번째 플레이어 컨트롤러를 가져옵니다.
        PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
        if (PlayerController)
        {
            // 게임만의 입력 모드를 설정하고 마우스 커서를 숨깁니다.
            FInputModeGameOnly InputModeData;
            PlayerController->SetInputMode(InputModeData);
            PlayerController->SetShowMouseCursor(false);
        }
    }
    // ReturnButton이 바인딩되어 있으면 클릭 이벤트를 제거합니다.
    if (ReturnButton && ReturnButton->OnClicked.IsBound())
    {
        ReturnButton->OnClicked.RemoveDynamic(this, &UBackToMenu::ReturnButtonClicked);
    }
    // 멀티플레이어 세션 서브시스템이 있고 세션 파괴 완료 이벤트가 바인딩되어 있으면 이벤트를 제거합니다.
    if (MultiplayerSessionsSubsystem && MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.IsBound())
    {
        MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.RemoveDynamic(this, &UBackToMenu::OnDestroySession);
    }
}

// ReturnButtonClicked 메서드
// 이 메서드는 Return 버튼이 클릭되었을 때의 동작을 정의합니다.
void UBackToMenu::ReturnButtonClicked()
{
    // ReturnButton을 비활성화합니다.
    ReturnButton->SetIsEnabled(false);

    // 게임의 세계를 가져옵니다.
    UWorld* World = GetWorld();
    if (World)
    {
        // 첫 번째 플레이어 컨트롤러를 가져옵니다.
        APlayerController* FirstPlayerController = World->GetFirstPlayerController();
        if (FirstPlayerController)
        {
            // 플레이어의 캐릭터를 LetsShootCharacter로 캐스팅합니다.
            ALetsShootCharacter* LetsShootCharacter = Cast<ALetsShootCharacter>(FirstPlayerController->GetPawn());
            if (LetsShootCharacter)
            {
                // 캐릭터가 존재하면 게임을 떠나도록 요청하고, 게임을 떠난 후의 동작을 설정합니다.
                LetsShootCharacter->ServerLeaveGame();
                LetsShootCharacter->OnLeftGame.AddDynamic(this, &UBackToMenu::OnPlayerLeftGame);
            }
            else
            {
                // 캐릭터가 존재하지 않으면 ReturnButton을 활성화합니다.
                ReturnButton->SetIsEnabled(true);
            }
        }
    }
}

// OnPlayerLeftGame 메서드
// 이 메서드는 플레이어가 게임을 떠났을 때의 동작을 정의합니다.
void UBackToMenu::OnPlayerLeftGame()
{
    // 플레이어가 게임을 떠난 것을 로그에 기록합니다.
    UE_LOG(LogTemp, Warning, TEXT("OnPlayerLeftGame()"))
        if (MultiplayerSessionsSubsystem)
        {
            // 멀티플레이어 세션 서브시스템이 존재하면 세션을 파괴합니다.
            UE_LOG(LogTemp, Warning, TEXT("MultiplayerSessionsSubsystem valid"))
                MultiplayerSessionsSubsystem->DestroySession();
        }
}


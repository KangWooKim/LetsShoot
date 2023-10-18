// Fill out your copyright notice in the Description page of Project Settings.


#include "BackToMenu.h"
#include "GameFramework/PlayerController.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "GameFramework/GameModeBase.h"
#include "LetsShoot/Character/LetsShootCharacter.h"

// UBackToMenu Ŭ������ �����մϴ�.

// MenuSetup �޼���
// �� �޼���� �޴� ������ ����մϴ�.
void UBackToMenu::MenuSetup()
{
    // �޴��� ����Ʈ�� �߰��ϰ�, �޴��� ���̰� �����ϸ�, ��Ŀ���� ���� �� �ֵ��� �����մϴ�.
    AddToViewport();
    SetVisibility(ESlateVisibility::Visible);
    bIsFocusable = true;

    // ������ ���踦 �����ɴϴ�.
    UWorld* World = GetWorld();
    if (World)
    {
        // PlayerController�� null�̸� ù ��° �÷��̾� ��Ʈ�ѷ��� �����ɴϴ�.
        PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
        if (PlayerController)
        {
            // �Է� ��带 �����ϰ�, ���콺 Ŀ���� ���̰� �մϴ�.
            FInputModeGameAndUI InputModeData;
            InputModeData.SetWidgetToFocus(TakeWidget());
            PlayerController->SetInputMode(InputModeData);
            PlayerController->SetShowMouseCursor(true);
        }
    }
    // ReturnButton�� ���ε��Ǿ� ���� ������ Ŭ�� �̺�Ʈ�� �߰��մϴ�.
    if (ReturnButton && !ReturnButton->OnClicked.IsBound())
    {
        ReturnButton->OnClicked.AddDynamic(this, &UBackToMenu::ReturnButtonClicked);
    }
    // ���� �ν��Ͻ��� �����ͼ� ��Ƽ�÷��̾� ���� ����ý����� �����ɴϴ�.
    UGameInstance* GameInstance = GetGameInstance();
    if (GameInstance)
    {
        MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
        if (MultiplayerSessionsSubsystem)
        {
            // ���� �ı� �Ϸ� �̺�Ʈ�� �����ʸ� �߰��մϴ�.
            MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &UBackToMenu::OnDestroySession);
        }
    }
}

// Initialize �޼���
// �� �޼���� �ʱ�ȭ�� ����մϴ�.
bool UBackToMenu::Initialize()
{
    // ���� Ŭ������ Initialize �޼��带 ȣ���Ͽ� �ʱ�ȭ�� �õ��մϴ�.
    if (!Super::Initialize())
    {
        // �ʱ�ȭ�� �����ϸ� false�� ��ȯ�մϴ�.
        return false;
    }

    // �ʱ�ȭ�� �����ϸ� true�� ��ȯ�մϴ�.
    return true;
}

// OnDestroySession �޼���
// �� �޼���� ���� �ı� �̺�Ʈ�� ó���մϴ�.
// �Ű�����:
// - bWasSuccessful: ���� �ı��� ���������� �̷�������� ��Ÿ���� bool ���Դϴ�.
void UBackToMenu::OnDestroySession(bool bWasSuccessful)
{
    // ���� �ı��� �����ϸ� ReturnButton�� Ȱ��ȭ�ϰ� �޼��带 �����մϴ�.
    if (!bWasSuccessful)
    {
        ReturnButton->SetIsEnabled(true);
        return;
    }

    // ������ ���踦 �����ɴϴ�.
    UWorld* World = GetWorld();
    if (World)
    {
        // ���� ��带 �����ɴϴ�.
        AGameModeBase* GameMode = World->GetAuthGameMode<AGameModeBase>();
        if (GameMode)
        {
            // ���� ��尡 ������ ���� �޴��� ���ư��ϴ�.
            GameMode->BackToMenuHost();
        }
        else
        {
            // PlayerController�� null�̸� ù ��° �÷��̾� ��Ʈ�ѷ��� �����ɴϴ�.
            PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
            if (PlayerController)
            {
                // PlayerController�� ������ Ŭ���̾�Ʈ�� ���� �޴��� ���������ϴ�.
                PlayerController->ClientBackToMenuWithTextReason(FText());
            }
        }
    }
}


// MenuTearDown �޼���
// �� �޼���� �޴��� �����ϴ� ������ �մϴ�.
void UBackToMenu::MenuTearDown()
{
    // �θ� �������κ��� ���� ������ �����մϴ�.
    RemoveFromParent();

    // ������ ���踦 �����ɴϴ�.
    UWorld* World = GetWorld();
    if (World)
    {
        // PlayerController�� null�̸� ù ��° �÷��̾� ��Ʈ�ѷ��� �����ɴϴ�.
        PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
        if (PlayerController)
        {
            // ���Ӹ��� �Է� ��带 �����ϰ� ���콺 Ŀ���� ����ϴ�.
            FInputModeGameOnly InputModeData;
            PlayerController->SetInputMode(InputModeData);
            PlayerController->SetShowMouseCursor(false);
        }
    }
    // ReturnButton�� ���ε��Ǿ� ������ Ŭ�� �̺�Ʈ�� �����մϴ�.
    if (ReturnButton && ReturnButton->OnClicked.IsBound())
    {
        ReturnButton->OnClicked.RemoveDynamic(this, &UBackToMenu::ReturnButtonClicked);
    }
    // ��Ƽ�÷��̾� ���� ����ý����� �ְ� ���� �ı� �Ϸ� �̺�Ʈ�� ���ε��Ǿ� ������ �̺�Ʈ�� �����մϴ�.
    if (MultiplayerSessionsSubsystem && MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.IsBound())
    {
        MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.RemoveDynamic(this, &UBackToMenu::OnDestroySession);
    }
}

// ReturnButtonClicked �޼���
// �� �޼���� Return ��ư�� Ŭ���Ǿ��� ���� ������ �����մϴ�.
void UBackToMenu::ReturnButtonClicked()
{
    // ReturnButton�� ��Ȱ��ȭ�մϴ�.
    ReturnButton->SetIsEnabled(false);

    // ������ ���踦 �����ɴϴ�.
    UWorld* World = GetWorld();
    if (World)
    {
        // ù ��° �÷��̾� ��Ʈ�ѷ��� �����ɴϴ�.
        APlayerController* FirstPlayerController = World->GetFirstPlayerController();
        if (FirstPlayerController)
        {
            // �÷��̾��� ĳ���͸� LetsShootCharacter�� ĳ�����մϴ�.
            ALetsShootCharacter* LetsShootCharacter = Cast<ALetsShootCharacter>(FirstPlayerController->GetPawn());
            if (LetsShootCharacter)
            {
                // ĳ���Ͱ� �����ϸ� ������ �������� ��û�ϰ�, ������ ���� ���� ������ �����մϴ�.
                LetsShootCharacter->ServerLeaveGame();
                LetsShootCharacter->OnLeftGame.AddDynamic(this, &UBackToMenu::OnPlayerLeftGame);
            }
            else
            {
                // ĳ���Ͱ� �������� ������ ReturnButton�� Ȱ��ȭ�մϴ�.
                ReturnButton->SetIsEnabled(true);
            }
        }
    }
}

// OnPlayerLeftGame �޼���
// �� �޼���� �÷��̾ ������ ������ ���� ������ �����մϴ�.
void UBackToMenu::OnPlayerLeftGame()
{
    // �÷��̾ ������ ���� ���� �α׿� ����մϴ�.
    UE_LOG(LogTemp, Warning, TEXT("OnPlayerLeftGame()"))
        if (MultiplayerSessionsSubsystem)
        {
            // ��Ƽ�÷��̾� ���� ����ý����� �����ϸ� ������ �ı��մϴ�.
            UE_LOG(LogTemp, Warning, TEXT("MultiplayerSessionsSubsystem valid"))
                MultiplayerSessionsSubsystem->DestroySession();
        }
}


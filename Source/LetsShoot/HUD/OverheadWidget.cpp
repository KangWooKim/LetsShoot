// Fill out your copyright notice in the Description page of Project Settings.


#include "OverheadWidget.h"
#include "Components/TextBlock.h"

// UOverheadWidget 클래스를 정의합니다.

// SetDisplayText 메서드
// 이 메서드는 텍스트를 설정하는 역할을 합니다.
// 매개변수:
// - TextToDisplay: 화면에 표시할 문자열을 FString 형식으로 전달받습니다.
void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
    // DisplayText가 null이 아닌 경우에만 실행합니다.
    if (DisplayText)
    {
        // DisplayText의 텍스트를 전달받은 문자열로 설정합니다.
        DisplayText->SetText(FText::FromString(TextToDisplay));
    }
}

// ShowPlayerNetRole 메서드
// 이 메서드는 플레이어의 네트워크 역할을 보여주는 역할을 합니다.
// 매개변수:
// - InPawn: 네트워크 역할을 조회할 APawn 객체를 전달받습니다.
void UOverheadWidget::ShowPlayerNetRole(APawn* InPawn)
{
    // InPawn의 RemoteRole 값을 조회합니다.
    ENetRole RemoteRole = InPawn->GetRemoteRole();
    FString Role;
    // RemoteRole의 값에 따라 Role 문자열을 설정합니다.
    switch (RemoteRole)
    {
    case ENetRole::ROLE_Authority:
        Role = FString("Authority");
        break;
    case ENetRole::ROLE_AutonomousProxy:
        Role = FString("Autonomous Proxy");
        break;
    case ENetRole::ROLE_SimulatedProxy:
        Role = FString("Simulated Proxy");
        break;
    case ENetRole::ROLE_None:
        Role = FString("None");
        break;
    }
    // Remote Role과 그에 대응하는 Role 문자열을 결합합니다.
    FString RemoteRoleString = FString::Printf(TEXT("Remote Role: %s"), *Role);
    // 결합한 문자열을 화면에 표시합니다.
    SetDisplayText(RemoteRoleString);
}

// NativeDestruct 메서드
// 이 메서드는 위젯을 부모 위젯으로부터 제거하고 기본 소멸자를 호출하는 역할을 합니다.
void UOverheadWidget::NativeDestruct()
{
    // 위젯을 부모 위젯으로부터 제거합니다.
    RemoveFromParent();
    // 기본 소멸자를 호출합니다.
    Super::NativeDestruct();
}


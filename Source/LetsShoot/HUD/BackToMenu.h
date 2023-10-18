// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BackToMenu.generated.h"

/**
 *
 */
UCLASS()
// UBackToMenuMenu 클래스
// 이 클래스는 메인 메뉴를 관리하는 위젯입니다.
class LETSSHOOT_API UBackToMenu : public UUserWidget
{
	// 이 클래스의 메타데이터를 생성합니다.
	GENERATED_BODY()

public:
	// 메뉴 설정 메서드입니다.
	void MenuSetup();

	// 메뉴 해제 메서드입니다.
	void MenuTearDown();

protected:
	// 초기화 메서드입니다.
	// 이 메서드는 UUserWidget 클래스의 Initialize 메서드를 오버라이드합니다.
	virtual bool Initialize() override;

	// 세션 파괴 메서드입니다.
	// 이 메서드는 세션이 성공적으로 파괴되었는지 여부를 매개변수로 받습니다.
	UFUNCTION()
		void OnDestroySession(bool bWasSuccessful);

	// 플레이어가 게임을 떠난 경우의 동작을 정의하는 메서드입니다.
	UFUNCTION()
		void OnPlayerLeftGame();

private:
	// Return 버튼입니다.
	// 이 변수는 UButton 클래스의 인스턴스를 참조합니다.
	UPROPERTY(meta = (BindWidget))
		class UButton* ReturnButton;

	// Return 버튼 클릭 메서드입니다.
	UFUNCTION()
		void ReturnButtonClicked();

	// 멀티플레이어 세션 서브시스템입니다.
	// 이 변수는 UMultiplayerSessionsSubsystem 클래스의 인스턴스를 참조합니다.
	UPROPERTY()
		class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	// 플레이어 컨트롤러입니다.
	// 이 변수는 APlayerController 클래스의 인스턴스를 참조합니다.
	UPROPERTY()
		class APlayerController* PlayerController;
};

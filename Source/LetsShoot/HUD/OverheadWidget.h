// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverheadWidget.generated.h"

/**
 * 
 */
UCLASS()
// UOverheadWidget 클래스를 선언합니다. 이 클래스는 UUserWidget을 상속받습니다.
class LETSSHOOT_API UOverheadWidget : public UUserWidget
{
    // 클래스 바디를 생성합니다.
    GENERATED_BODY()

public:
    // DisplayText라는 UTextBlock 포인터 변수를 선언합니다. 
    // 이 변수는 위젯에 표시될 텍스트를 관리합니다.
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* DisplayText;

    // SetDisplayText 메서드를 선언합니다.
    // 이 메서드는 위젯에 표시될 텍스트를 설정합니다.
    // 매개변수:
    // - TextToDisplay: 화면에 표시할 문자열을 FString 형식으로 전달받습니다.
    void SetDisplayText(FString TextToDisplay);

    // ShowPlayerNetRole 메서드를 선언합니다.
    // 이 메서드는 블루프린트에서 호출 가능하며, 플레이어의 네트워크 역할을 보여줍니다.
    // 매개변수:
    // - InPawn: 네트워크 역할을 조회할 APawn 객체를 전달받습니다.
    UFUNCTION(BlueprintCallable)
    void ShowPlayerNetRole(APawn* InPawn);

protected:
    // NativeDestruct 메서드를 선언합니다.
    // 이 메서드는 위젯을 부모 위젯으로부터 제거하고 기본 소멸자를 호출합니다.
    virtual void NativeDestruct() override;

};

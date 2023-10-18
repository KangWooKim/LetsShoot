// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DieAnnouncement.generated.h"

/**
 * 
 */
UCLASS()
// UDieAnnouncement 클래스는 제거 알림을 표시하는 사용자 위젯입니다.
class LETSSHOOT_API UDieAnnouncement : public UUserWidget
{
	GENERATED_BODY()
public:
    // 공격자와 피해자의 이름을 인자로 받아 제거 알림 텍스트를 설정합니다.
	void SetDieAnnouncementText(FString AttackerName, FString VictimName);

    // UHorizontalBox를 참조하는 변수로 알림 메시지를 포함하는 박스입니다.
	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* AnnouncementBox;

    // UTextBlock을 참조하는 변수로 실제 알림 텍스트를 포함합니다.
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AnnouncementText;

};

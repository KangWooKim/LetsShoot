// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Announcement.generated.h"

/**
 * 
 */
UCLASS()
// LetsShoot 게임의 공지사항 UI 위젯 클래스
class LETSSHOOT_API UAnnouncement : public UUserWidget
{
	GENERATED_BODY()
public:

	// WarmupTime 시간을 보여주는 텍스트 블록
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* WarmupTime;

	// 공지사항을 보여주는 텍스트 블록
	UPROPERTY(meta = (BindWidget))
		UTextBlock* AnnouncementText;

	// 추가 정보를 보여주는 텍스트 블록
	UPROPERTY(meta = (BindWidget))
		UTextBlock* InfoText;

};


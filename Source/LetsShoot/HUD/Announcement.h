// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Announcement.generated.h"

/**
 * 
 */
UCLASS()
// LetsShoot ������ �������� UI ���� Ŭ����
class LETSSHOOT_API UAnnouncement : public UUserWidget
{
	GENERATED_BODY()
public:

	// WarmupTime �ð��� �����ִ� �ؽ�Ʈ ���
	UPROPERTY(meta = (BindWidget))
		class UTextBlock* WarmupTime;

	// ���������� �����ִ� �ؽ�Ʈ ���
	UPROPERTY(meta = (BindWidget))
		UTextBlock* AnnouncementText;

	// �߰� ������ �����ִ� �ؽ�Ʈ ���
	UPROPERTY(meta = (BindWidget))
		UTextBlock* InfoText;

};


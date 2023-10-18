// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DieAnnouncement.generated.h"

/**
 * 
 */
UCLASS()
// UDieAnnouncement Ŭ������ ���� �˸��� ǥ���ϴ� ����� �����Դϴ�.
class LETSSHOOT_API UDieAnnouncement : public UUserWidget
{
	GENERATED_BODY()
public:
    // �����ڿ� �������� �̸��� ���ڷ� �޾� ���� �˸� �ؽ�Ʈ�� �����մϴ�.
	void SetDieAnnouncementText(FString AttackerName, FString VictimName);

    // UHorizontalBox�� �����ϴ� ������ �˸� �޽����� �����ϴ� �ڽ��Դϴ�.
	UPROPERTY(meta = (BindWidget))
	class UHorizontalBox* AnnouncementBox;

    // UTextBlock�� �����ϴ� ������ ���� �˸� �ؽ�Ʈ�� �����մϴ�.
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* AnnouncementText;

};

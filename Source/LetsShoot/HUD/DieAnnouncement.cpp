// Fill out your copyright notice in the Description page of Project Settings.


#include "DieAnnouncement.h"
#include "Components/TextBlock.h"

// UDieAnnouncement Ŭ������ Ư�� �÷��̾ �ٸ� �÷��̾ �����ߴٴ� ����� �˸��� �޽����� �����ϴ� �޼��带 ������ �ֽ��ϴ�.
void UDieAnnouncement::SetDieAnnouncementText(FString AttackerName, FString VictimName) // �޼����� ���ڷ� �����ڿ� ������� �̸��� �޽��ϴ�.
{
	// �����ڰ� ����ڸ� �����ߴٴ� ���ڿ��� �����մϴ�. 
	FString DieAnnouncementText = FString::Printf(TEXT("%s elimmed %s!"), *AttackerName, *VictimName);

	// AnnouncementText�� null�� �ƴ� ��쿡�� �ؽ�Ʈ�� �����մϴ�.
	// �̴� �츮�� ����� �������̽� ��Ҹ� �����ϰ� �����ϰ� �ִ��� Ȯ���ϴ� ���� �߿��ϱ� �����Դϴ�.
	if (AnnouncementText)
	{
		// AnnouncementText ������ �ؽ�Ʈ�� DieAnnouncementText�� �����մϴ�.
		AnnouncementText->SetText(FText::FromString(DieAnnouncementText));
	}
}

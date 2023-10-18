// Fill out your copyright notice in the Description page of Project Settings.


#include "DieAnnouncement.h"
#include "Components/TextBlock.h"

// UDieAnnouncement 클래스는 특정 플레이어가 다른 플레이어를 제거했다는 사실을 알리는 메시지를 설정하는 메서드를 가지고 있습니다.
void UDieAnnouncement::SetDieAnnouncementText(FString AttackerName, FString VictimName) // 메서드의 인자로 공격자와 희생자의 이름을 받습니다.
{
	// 공격자가 희생자를 제거했다는 문자열을 생성합니다. 
	FString DieAnnouncementText = FString::Printf(TEXT("%s elimmed %s!"), *AttackerName, *VictimName);

	// AnnouncementText가 null이 아닌 경우에만 텍스트를 설정합니다.
	// 이는 우리가 사용자 인터페이스 요소를 적절하게 참조하고 있는지 확인하는 것이 중요하기 때문입니다.
	if (AnnouncementText)
	{
		// AnnouncementText 위젯의 텍스트를 DieAnnouncementText로 설정합니다.
		AnnouncementText->SetText(FText::FromString(DieAnnouncementText));
	}
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "LetsShootHUD.h"
#include "GameFramework/PlayerController.h"
#include "CharacterOverlay.h"
#include "Announcement.h"
#include "DieAnnouncement.h"
#include "Components/HorizontalBox.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"

// ALetsShootHUD 클래스의 BeginPlay 메서드입니다. 게임 플레이가 시작될 때 호출됩니다.
void ALetsShootHUD::BeginPlay()
{
	Super::BeginPlay();  // 부모 클래스의 BeginPlay 메서드를 호출합니다.
}

// Attacker가 Victim을 제거했음을 알리는 메시지를 화면에 표시하는 메서드입니다.
void ALetsShootHUD::AddElimAnnouncement(FString Attacker, FString Victim)  // Attacker: 공격자 이름, Victim: 피해자 이름
{
	OwningPlayer = OwningPlayer == nullptr ? GetOwningPlayerController() : OwningPlayer;  // OwningPlayer가 null이면 GetOwningPlayerController로 가져옵니다.
	if (OwningPlayer && ElimAnnouncementClass)  // OwningPlayer와 ElimAnnouncementClass가 유효한지 확인합니다.
	{
		// ElimAnnouncementClass 유형의 위젯을 생성합니다.
		UElimAnnouncement* ElimAnnouncementWidget = CreateWidget<UElimAnnouncement>(OwningPlayer, ElimAnnouncementClass);
		if (ElimAnnouncementWidget)  // 위젯 생성이 성공적인지 확인합니다.
		{
			// 위젯에 텍스트를 설정하고 화면에 표시합니다.
			ElimAnnouncementWidget->SetElimAnnouncementText(Attacker, Victim);
			ElimAnnouncementWidget->AddToViewport();

			// 이미 표시된 메시지를 위로 이동합니다.
			for (UElimAnnouncement* Msg : ElimMessages)
			{
				if (Msg && Msg->AnnouncementBox)
				{
					UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Msg->AnnouncementBox);
					if (CanvasSlot)
					{
						FVector2D Position = CanvasSlot->GetPosition();
						FVector2D NewPosition(
							CanvasSlot->GetPosition().X,
							Position.Y - CanvasSlot->GetSize().Y
						);
						CanvasSlot->SetPosition(NewPosition);
					}
				}
			}

			// 메시지를 목록에 추가합니다.
			ElimMessages.Add(ElimAnnouncementWidget);

			// 일정 시간 후 메시지를 제거하는 타이머를 설정합니다.
			FTimerHandle ElimMsgTimer;
			FTimerDelegate ElimMsgDelegate;
			ElimMsgDelegate.BindUFunction(this, FName("ElimAnnouncementTimerFinished"), ElimAnnouncementWidget);
			GetWorldTimerManager().SetTimer(
				ElimMsgTimer,
				ElimMsgDelegate,
				ElimAnnouncementTime,
				false
			);
		}
	}
}

// ElimAnnouncement 메시지의 타이머가 끝났을 때 호출되는 메서드입니다.
void ALetsShootHUD::ElimAnnouncementTimerFinished(UElimAnnouncement* MsgToRemove)  // MsgToRemove: 제거할 메시지 위젯
{
	if (MsgToRemove)  // MsgToRemove가 유효한지 확인합니다.
	{
		MsgToRemove->RemoveFromParent();  // 유효하다면 부모 위젯으로부터 제거합니다.
	}
}

// 캐릭터 오버레이 위젯을 화면에 추가하는 메서드입니다.
void ALetsShootHUD::AddCharacterOverlay()
{
	APlayerController* PlayerController = GetOwningPlayerController();  // 플레이어 컨트롤러를 가져옵니다.
	if (PlayerController && CharacterOverlayClass)  // PlayerController와 CharacterOverlayClass가 유효한지 확인합니다.
	{
		// CharacterOverlayClass 유형의 위젯을 생성하고 화면에 표시합니다.
		CharacterOverlay = CreateWidget<UCharacterOverlay>(PlayerController, CharacterOverlayClass);
		CharacterOverlay->AddToViewport();
	}
}

// ALetsShootHUD 클래스의 AddAnnouncement 메서드입니다. 플레이어에게 알림을 표시하는 위젯을 생성하고 화면에 추가합니다.
void ALetsShootHUD::AddAnnouncement()
{
	APlayerController* PlayerController = GetOwningPlayerController();  // 플레이어 컨트롤러를 가져옵니다.
	if (PlayerController && AnnouncementClass)  // PlayerController와 AnnouncementClass가 유효한지 확인합니다.
	{
		// AnnouncementClass 유형의 위젯을 생성하고 화면에 추가합니다.
		Announcement = CreateWidget<UAnnouncement>(PlayerController, AnnouncementClass);
		Announcement->AddToViewport();
	}
}

// 화면에 HUD (Head-Up Display, 헤드업 디스플레이)를 그리는 메서드입니다. 이는 플레이어에게 게임의 중요한 정보를 보여주는 역할을 합니다.
void ALetsShootHUD::DrawHUD()
{
	Super::DrawHUD();  // 부모 클래스의 DrawHUD 메서드를 호출합니다.

	FVector2D ViewportSize;  // 뷰포트의 크기를 저장할 변수입니다.
	if (GEngine)  // 게임 엔진이 유효한지 확인합니다.
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);  // 게임 엔진을 통해 뷰포트의 크기를 가져옵니다.
		const FVector2D ViewportCenter(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);  // 뷰포트의 중심 좌표를 계산합니다.

		float SpreadScaled = CrosshairSpreadMax * HUDPackage.CrosshairSpread;  // 조준점의 확산을 계산합니다.

		// HUDPackage에 포함된 각 측면의 조준점을 그립니다.
		// 조준점이 유효한 경우에만 그립니다.
		if (HUDPackage.CrosshairsCenter)
		{
			FVector2D Spread(0.f, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsCenter, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
		}
		if (HUDPackage.CrosshairsLeft)
		{
			FVector2D Spread(-SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsLeft, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
		}
		if (HUDPackage.CrosshairsRight)
		{
			FVector2D Spread(SpreadScaled, 0.f);
			DrawCrosshair(HUDPackage.CrosshairsRight, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
		}
		if (HUDPackage.CrosshairsTop)
		{
			FVector2D Spread(0.f, -SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairsTop, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
		}
		if (HUDPackage.CrosshairsBottom)
		{
			FVector2D Spread(0.f, SpreadScaled);
			DrawCrosshair(HUDPackage.CrosshairsBottom, ViewportCenter, Spread, HUDPackage.CrosshairsColor);
		}
	}
}

// 조준점을 그리는 메서드입니다.
void ALetsShootHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor)
{
	const float TextureWidth = Texture->GetSizeX();  // 텍스처의 너비를 가져옵니다.
	const float TextureHeight = Texture->GetSizeY();  // 텍스처의 높이를 가져옵니다.
	const FVector2D TextureDrawPoint(  // 텍스처를 그릴 위치를 계산합니다.
		ViewportCenter.X - (TextureWidth / 2.f) + Spread.X,
		ViewportCenter.Y - (TextureHeight / 2.f) + Spread.Y
	);

	// 텍스처를 화면에 그립니다.
	DrawTexture(
		Texture,
		TextureDrawPoint.X,
		TextureDrawPoint.Y,
		TextureWidth,
		TextureHeight,
		0.f,
		0.f,
		1.f,
		1.f,
		CrosshairColor
	);
}

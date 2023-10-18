// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LetsShootHUD.generated.h"

USTRUCT(BlueprintType)
// HUD를 구성하는 요소들을 담는 구조체입니다.
struct FHUDPackage
{
	GENERATED_BODY()
public:
	class UTexture2D* CrosshairsCenter;  // 중앙의 조준점 텍스처를 나타냅니다.
	UTexture2D* CrosshairsLeft;  // 왼쪽의 조준점 텍스처를 나타냅니다.
	UTexture2D* CrosshairsRight;  // 오른쪽의 조준점 텍스처를 나타냅니다.
	UTexture2D* CrosshairsTop;  // 상단의 조준점 텍스처를 나타냅니다.
	UTexture2D* CrosshairsBottom;  // 하단의 조준점 텍스처를 나타냅니다.
	float CrosshairSpread;  // 조준점의 확산도를 나타냅니다.
	FLinearColor CrosshairsColor;  // 조준점의 색상을 나타냅니다.
};

/**
 * 
 */
UCLASS()
class LETSSHOOT_API ALetsShootHUD : public AHUD
{
	GENERATED_BODY()
public:
	virtual void DrawHUD() override;  // 화면에 HUD를 그리는 메서드입니다.

	// 플레이어의 상태를 표시하는 위젯의 클래스입니다.
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<class UUserWidget> CharacterOverlayClass;

	void AddCharacterOverlay();  // 플레이어의 상태를 화면에 표시하는 위젯을 추가하는 메서드입니다.

	// 플레이어의 상태를 표시하는 위젯의 인스턴스입니다.
	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay;

	// 알림을 표시하는 위젯의 클래스입니다.
	UPROPERTY(EditAnywhere, Category = "Announcements")
	TSubclassOf<UUserWidget> AnnouncementClass;

	// 알림을 표시하는 위젯의 인스턴스입니다.
	UPROPERTY()
	class UAnnouncement* Announcement;

	void AddAnnouncement();  // 알림을 화면에 표시하는 위젯을 추가하는 메서드입니다.
	void AddElimAnnouncement(FString Attacker, FString Victim);  // 탈락 알림을 추가하는 메서드입니다. Attacker와 Victim은 각각 공격자와 피해자를 나타냅니다.
protected:
	virtual void BeginPlay() override;  // 게임이 시작될 때 호출되는 메서드입니다.

private:
	// 플레이어 컨트롤러의 참조입니다. 
	UPROPERTY()
		class APlayerController* OwningPlayer;

	// HUD 요소들을 담고 있는 패키지입니다.
	FHUDPackage HUDPackage;

	// 텍스처, 뷰포트 중심 위치, 확산, 색상을 인자로 받아서 조준점을 그리는 메서드입니다.
	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairColor);

	// 조준점의 최대 확산 크기입니다. 기본값은 16입니다.
	UPROPERTY(EditAnywhere)
		float CrosshairSpreadMax = 16.f;

	// 탈락 알림 위젯의 클래스입니다.
	UPROPERTY(EditAnywhere)
		TSubclassOf<class UElimAnnouncement> ElimAnnouncementClass;

	// 탈락 알림이 표시되는 시간입니다. 기본값은 2.5초입니다.
	UPROPERTY(EditAnywhere)
		float ElimAnnouncementTime = 2.5f;

	// 탈락 알림 타이머가 끝났을 때 호출되는 메서드입니다. 삭제할 메시지를 인자로 받습니다.
	UFUNCTION()
		void ElimAnnouncementTimerFinished(UElimAnnouncement* MsgToRemove);

	// 화면에 표시된 탈락 알림 메시지들의 배열입니다.
	UPROPERTY()
		TArray<UElimAnnouncement*> ElimMessages;

public:
	// HUD 패키지를 설정하는 메서드입니다. 설정할 패키지를 인자로 받습니다.
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; }

};

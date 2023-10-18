// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LetsShootCharacterOverlay.generated.h"

/**
 * 
 */
UCLASS()
// 캐릭터 오버레이를 위한 사용자 위젯을 정의한 클래스입니다.
class LETSSHOOT_API ULetsShootCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()
public:
	// 캐릭터의 체력바입니다.
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	// 캐릭터의 체력을 표시하는 텍스트입니다.
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HealthText;

	// 캐릭터의 보호막 바입니다.
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ShieldBar;

	// 캐릭터의 보호막을 표시하는 텍스트입니다.
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ShieldText;

	// 캐릭터의 점수를 표시하는 텍스트입니다.
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreAmount;

	// 레드팀의 점수를 표시하는 텍스트입니다.
	UPROPERTY(meta = (BindWidget))
	UTextBlock* RedTeamScore;

	// 블루팀의 점수를 표시하는 텍스트입니다.
	UPROPERTY(meta = (BindWidget))
	UTextBlock* BlueTeamScore;

	// 점수의 간격을 표시하는 텍스트입니다.
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreSpacerText;

	// 캐릭터의 패배 횟수를 표시하는 텍스트입니다.
	UPROPERTY(meta = (BindWidget))
	UTextBlock* DefeatsAmount;

	// 캐릭터가 가지고 있는 무기의 탄약을 표시하는 텍스트입니다.
	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeaponAmmoAmount;

	// 캐릭터가 소지하고 있는 추가 탄약을 표시하는 텍스트입니다.
	UPROPERTY(meta = (BindWidget))
	UTextBlock* CarriedAmmoAmount;

	// 경기의 남은 시간을 표시하는 텍스트입니다.
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MatchCountdownText;

	// 캐릭터가 가지고 있는 수류탄의 수를 표시하는 텍스트입니다.
	UPROPERTY(meta = (BindWidget))
	UTextBlock* GrenadesText;

	// 높은 핑을 나타내는 이미지입니다.
	UPROPERTY(meta = (BindWidget))
	class UImage* HighPingImage;

	// 높은 핑을 애니메이션으로 나타냅니다.
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* HighPingAnimation;
};


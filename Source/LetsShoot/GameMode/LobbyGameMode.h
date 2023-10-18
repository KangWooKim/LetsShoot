// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class LETSSHOOT_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	// 새로운 플레이어가 게임에 로그인했을 때 호출되는 함수
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
};

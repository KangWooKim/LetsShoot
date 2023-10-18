// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "LetsShoot/LetsShootTypes/TeamColor.h"
#include "TeamFightPlayerStart.generated.h"

/**
 * 
 */
UCLASS()
class LETSSHOOT_API ATeamFightPlayerStart : public APlayerStart
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	ETeamColor Team;
};

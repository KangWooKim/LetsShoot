// Fill out your copyright notice in the Description page of Project Settings.


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LetsShoot/LetsShootTypes/Team.h"
#include "FlagZone.generated.h"

UCLASS()
class LETSSHOOT_API AFlagZone : public AActor
{
	GENERATED_BODY()

public:
	// AFlagZone 생성자
	AFlagZone();

	// 팀 정보 변수. 에디터에서 변경 가능
	UPROPERTY(EditAnywhere)
		ETeam Team;

protected:
	// 게임이 시작될 때 호출되는 메서드
	virtual void BeginPlay() override;

	// ZoneSphere 컴포넌트가 다른 컴포넌트와 오버랩되면 호출되는 메서드
	// @param OverlappedComponent: 오버랩 이벤트가 발생한 컴포넌트
	// @param OtherActor: 오버랩된 다른 액터
	// @param OtherComp: 오버랩된 다른 컴포넌트
	// @param OtherBodyIndex: 다른 컴포넌트의 바디 인덱스
	// @param bFromSweep: 스윕 결과에서 오버랩이 발생했는지의 여부
	// @param SweepResult: 스윕의 결과를 포함하고 있는 구조체
	UFUNCTION()
		virtual void OnSphereOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult
		);

private:

	// 플래그 존을 나타내는 스피어 컴포넌트. 에디터에서 변경 가능
	UPROPERTY(EditAnywhere)
		class USphereComponent* ZoneSphere;

public:

};

// Fill out your copyright notice in the Description page of Project Settings.


#include "FlagZone.h"
#include "Components/SphereComponent.h"
#include "LetsShoot/Weapon/Flag.h"
#include "LetsShoot/GameMode/CaptureTheFlagGameMode.h"
#include "LetsShoot/Character/LetsShootCharacter.h"

// AFlagZone 생성자
AFlagZone::AFlagZone()
{
	// 이 액터가 틱을 받을 수 없도록 설정
	PrimaryActorTick.bCanEverTick = false;

	// "ZoneSphere"라는 이름의 USphereComponent를 생성하고 ZoneSphere 변수에 할당
	ZoneSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ZoneSphere"));
	// 이 액터의 루트 컴포넌트를 ZoneSphere로 설정
	SetRootComponent(ZoneSphere);
}

// BeginPlay 메서드: 게임이 시작될 때 호출됨
void AFlagZone::BeginPlay()
{
	// 부모 클래스의 BeginPlay 메서드 호출
	Super::BeginPlay();

	// ZoneSphere의 오버랩 이벤트에 OnSphereOverlap 메서드를 바인드
	ZoneSphere->OnComponentBeginOverlap.AddDynamic(this, &AFlagZone::OnSphereOverlap);
}

// OnSphereOverlap 메서드: ZoneSphere가 다른 컴포넌트와 오버랩될 때 호출됨
// @param OverlappedComponent: 오버랩 이벤트가 발생한 컴포넌트
// @param OtherActor: 오버랩된 다른 액터
// @param OtherComp: 오버랩된 다른 컴포넌트
// @param OtherBodyIndex: 다른 컴포넌트의 바디 인덱스
// @param bFromSweep: 스윕 결과에서 오버랩이 발생했는지의 여부
// @param SweepResult: 스윕의 결과를 포함하고 있는 구조체
void AFlagZone::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 오버랩된 액터가 AFlag 타입인지 확인
	AFlag* OverlappingFlag = Cast<AFlag>(OtherActor);
	// 오버랩된 액터가 AFlag 타입이며, 그 팀이 현재 플래그 존의 팀이 아니라면
	if (OverlappingFlag && OverlappingFlag->GetTeam() != Team)
	{
		// 현재 월드의 게임 모드를 ACaptureTheFlagGameMode 타입으로 가져옴
		ACaptureTheFlagGameMode* GameMode = GetWorld()->GetAuthGameMode<ACaptureTheFlagGameMode>();
		// 게임 모드가 존재하면 플래그가 포착되었다는 이벤트를 호출
		if (GameMode)
		{
			GameMode->FlagCaptured(OverlappingFlag, this);
		}
		// 오버랩된 플래그를 리셋
		OverlappingFlag->ResetFlag();
	}
}


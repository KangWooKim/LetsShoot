// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DelayProcessingComponent.generated.h"

USTRUCT(BlueprintType)
struct FBoxInformation
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location; // 히트 박스의 위치를 나타냅니다.

	UPROPERTY()
	FRotator Rotation; // 히트 박스의 회전값을 나타냅니다.

	UPROPERTY()
	FVector BoxExtent; // 히트 박스의 크기를 나타냅니다.
};

USTRUCT(BlueprintType)
struct FFramePackage
{
	GENERATED_BODY()

	UPROPERTY()
	float Time; // 프레임 패키지의 시간을 나타냅니다.

	UPROPERTY()
	TMap<FName, FBoxInformation> HitBoxInfo; // 히트 박스 정보를 저장합니다.

	UPROPERTY()
	ALetsShootCharacter* Character; // 프레임 패키지와 관련된 캐릭터를 저장합니다.
};

USTRUCT(BlueprintType)
struct FServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	bool bHitConfirmed; // 타격 확인 여부를 나타냅니다.

	UPROPERTY()
	bool bHeadShot; // 헤드샷 여부를 나타냅니다.
};

USTRUCT(BlueprintType)
struct FShotgunServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<ALetsShootCharacter*, uint32> HeadShots; // 총기 사용 시 헤드샷 횟수를 저장합니다.

	UPROPERTY()
	TMap<ALetsShootCharacter*, uint32> BodyShots; // 총기 사용 시 몸통 샷 횟수를 저장합니다.

};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LETSSHOOT_API UDelayProcessingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// 생성자
	UDelayProcessingComponent();

	// ALetsShootCharacter 클래스의 friend 클래스
	friend class ALetsShootCharacter;

	// 컴포넌트의 Tick 함수
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 프레임 패키지를 보여주는 함수
	void ShowFramePackage(const FFramePackage& Package, const FColor& Color);

	/**
	* Hitscan
	*/

	// 서버 사이드 리와인드 함수
	// 인자:
	//   - HitCharacter: 타격을 받은 ALetsShootCharacter 객체
	//   - TraceStart: 레이캐스트의 시작 위치 (FVector_NetQuantize)
	//   - HitLocation: 타격 위치 (FVector_NetQuantize)
	//   - HitTime: 타격 시간
	FServerSideRewindResult ServerSideRewind(
		class ALetsShootCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize& HitLocation,
		float HitTime);

	/**
	* Projectile
	*/

	// 프로젝타일 서버 사이드 리와인드 함수
	// 인자:
	//   - HitCharacter: 타격을 받은 ALetsShootCharacter 객체
	//   - TraceStart: 레이캐스트의 시작 위치 (FVector_NetQuantize)
	//   - InitialVelocity: 초기 속도 (FVector_NetQuantize100)
	//   - HitTime: 타격 시간
	FServerSideRewindResult ProjectileServerSideRewind(
		ALetsShootCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize100& InitialVelocity,
		float HitTime
	);

	/**
	* Shotgun
	*/

	// 샷건 서버 사이드 리와인드 함수
	// 인자:
	//   - HitCharacters: 타격을 받은 ALetsShootCharacter 객체들의 배열
	//   - TraceStart: 레이캐스트의 시작 위치 (FVector_NetQuantize)
	//   - HitLocations: 타격 위치들의 배열 (FVector_NetQuantize)
	//   - HitTime: 타격 시간
	FShotgunServerSideRewindResult ShotgunServerSideRewind(
		const TArray<ALetsShootCharacter*>& HitCharacters,
		const FVector_NetQuantize& TraceStart,
		const TArray<FVector_NetQuantize>& HitLocations,
		float HitTime);

	// 서버에서 점수 요청하는 함수
	// 인자:
	//   - HitCharacter: 타격을 받은 ALetsShootCharacter 객체
	//   - TraceStart: 레이캐스트의 시작 위치 (FVector_NetQuantize)
	//   - HitLocation: 타격 위치 (FVector_NetQuantize)
	//   - HitTime: 타격 시간
	UFUNCTION(Server, Reliable)
		void ServerScoreRequest(
			ALetsShootCharacter* HitCharacter,
			const FVector_NetQuantize& TraceStart,
			const FVector_NetQuantize& HitLocation,
			float HitTime
		);

	// 프로젝타일 서버에서 점수 요청하는 함수
	// 인자:
	//   - HitCharacter: 타격을 받은 ALetsShootCharacter 객체
	//   - TraceStart: 레이캐스트의 시작 위치 (FVector_NetQuantize)
	//   - InitialVelocity: 초기 속도 (FVector_NetQuantize100)
	//   - HitTime: 타격 시간
	UFUNCTION(Server, Reliable)
		void ProjectileServerScoreRequest(
			ALetsShootCharacter* HitCharacter,
			const FVector_NetQuantize& TraceStart,
			const FVector_NetQuantize100& InitialVelocity,
			float HitTime
		);

	// 샷건 서버에서 점수 요청하는 함수
	// 인자:
	//   - HitCharacters: 타격을 받은 ALetsShootCharacter 객체들의 배열
	//   - TraceStart: 레이캐스트의 시작 위치 (FVector_NetQuantize)
	//   - HitLocations: 타격 위치들의 배열 (FVector_NetQuantize)
	//   - HitTime: 타격 시간
	UFUNCTION(Server, Reliable)
		void ShotgunServerScoreRequest(
			const TArray<ALetsShootCharacter*>& HitCharacters,
			const FVector_NetQuantize& TraceStart,
			const TArray<FVector_NetQuantize>& HitLocations,
			float HitTime
		);
protected:
	// 게임 시작 시 호출되는 함수
	virtual void BeginPlay() override;

	// 프레임 패키지를 저장하는 함수
	// 인자:
	//   - Package: 저장할 프레임 패키지 (FFramePackage)
	void SaveFramePackage(FFramePackage& Package);

	// 두 프레임 사이를 보간하여 타격 시간에 해당하는 프레임 패키지를 반환하는 함수
	// 인자:
	//   - OlderFrame: 이전 프레임 패키지 (FFramePackage)
	//   - YoungerFrame: 다음 프레임 패키지 (FFramePackage)
	//   - HitTime: 타격 시간
	FFramePackage InterpBetweenFrames(const FFramePackage& OlderFrame, const FFramePackage& YoungerFrame, float HitTime);

	// HitCharacter의 박스 위치를 캐시하는 함수
	// 인자:
	//   - HitCharacter: 박스 위치를 캐시할 ALetsShootCharacter 객체
	//   - OutFramePackage: 박스 위치를 저장할 프레임 패키지 (FFramePackage)
	void CacheBoxPositions(ALetsShootCharacter* HitCharacter, FFramePackage& OutFramePackage);

	// HitCharacter의 박스들을 이동시키는 함수
	// 인자:
	//   - HitCharacter: 이동할 박스들을 가지고 있는 ALetsShootCharacter 객체
	//   - Package: 이동에 사용할 프레임 패키지 (FFramePackage)
	void MoveBoxes(ALetsShootCharacter* HitCharacter, const FFramePackage& Package);

	// HitCharacter의 박스들을 초기 위치로 재설정하는 함수
	// 인자:
	//   - HitCharacter: 초기 위치로 재설정할 박스들을 가지고 있는 ALetsShootCharacter 객체
	//   - Package: 초기 위치를 사용할 프레임 패키지 (FFramePackage)
	void ResetHitBoxes(ALetsShootCharacter* HitCharacter, const FFramePackage& Package);

	// HitCharacter의 메시 충돌을 활성화하는 함수
	// 인자:
	//   - HitCharacter: 메시 충돌을 활성화할 ALetsShootCharacter 객체
	//   - CollisionEnabled: 활성화할 충돌 유형 (ECollisionEnabled::Type)
	void EnableCharacterMeshCollision(ALetsShootCharacter* HitCharacter, ECollisionEnabled::Type CollisionEnabled);

	// 프레임 패키지를 저장하는 함수
	void SaveFramePackage();

	// HitCharacter의 특정 시간에 해당하는 프레임 패키지를 반환하는 함수
	// 인자:
	//   - HitCharacter: 프레임 패키지를 확인할 ALetsShootCharacter 객체
	//   - HitTime: 타격 시간
	FFramePackage GetFrameToCheck(ALetsShootCharacter* HitCharacter, float HitTime);

	/**
	* Hitscan
	*/

	// 타격을 확인하는 함수
	// 인자:
	//   - Package: 프레임 패키지 (FFramePackage)
	//   - HitCharacter: 타격을 받은 ALetsShootCharacter 객체
	//   - TraceStart: 레이캐스트의 시작 위치 (FVector_NetQuantize)
	//   - HitLocation: 타격 위치 (FVector_NetQuantize)
	FServerSideRewindResult ConfirmHit(
		const FFramePackage& Package,
		ALetsShootCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize& HitLocation);

	/**
	* Projectile
	*/

	// 프로젝타일 타격을 확인하는 함수
	// 인자:
	//   - Package: 프레임 패키지 (FFramePackage)
	//   - HitCharacter: 타격을 받은 ALetsShootCharacter 객체
	//   - TraceStart: 레이캐스트의 시작 위치 (FVector_NetQuantize)
	//   - InitialVelocity: 초기 속도 (FVector_NetQuantize100)
	//   - HitTime: 타격 시간
	FServerSideRewindResult ProjectileConfirmHit(
		const FFramePackage& Package,
		ALetsShootCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize100& InitialVelocity,
		float HitTime
	);

	/**
	* Shotgun
	*/

	// 샷건 타격을 확인하는 함수
	// 인자:
	//   - FramePackages: 프레임 패키지들의 배열 (TArray<FFramePackage>)
	//   - TraceStart: 레이캐스트의 시작 위치 (FVector_NetQuantize)
	//   - HitLocations: 타격 위치들의 배열 (TArray<FVector_NetQuantize>)
	FShotgunServerSideRewindResult ShotgunConfirmHit(
		const TArray<FFramePackage>& FramePackages,
		const FVector_NetQuantize& TraceStart,
		const TArray<FVector_NetQuantize>& HitLocations
	);
private:

	// 캐릭터 객체
	UPROPERTY()
	ALetsShootCharacter* Character;

	// 플레이어 컨트롤러 객체
	UPROPERTY()
	class ALetsShootPlayerController* Controller;

	// 프레임 기록을 위한 이중 연결 리스트
	TDoubleLinkedList<FFramePackage> FrameHistory;

	// 최대 기록 시간
	UPROPERTY(EditAnywhere)
	float MaxRecordTime = 4.f;


public:	
	
		
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponTypes.h"
#include "LetsShoot/LetsShootTypes/Team.h"
#include "Weapon.generated.h"

// 무기의 상태를 나타내는 열거형
UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial State"), // 초기 상태
	EWS_Equipped UMETA(DisplayName = "Equipped"), // 장착 상태
	EWS_EquippedSecondary UMETA(DisplayName = "Equipped Secondary"), // 보조 장착 상태
	EWS_Dropped UMETA(DisplayName = "Dropped"), // 드롭 상태

	EWS_MAX UMETA(DisplayName = "DefaultMAX") // 상태 최대치
};

// 발사 타입을 나타내는 열거형
UENUM(BlueprintType)
enum class EFireType : uint8
{
	EFT_HitScan UMETA(DisplayName = "Hit Scan Weapon"), // 히트 스캔 무기 (즉시 명중)
	EFT_Projectile UMETA(DisplayName = "Projectile Weapon"), // 발사체 무기 (실제 투사체 발사)
	EFT_Shotgun UMETA(DisplayName = "Shotgun Weapon"), // 산탄총 무기 (여러 발사체 동시 발사)

	EFT_MAX UMETA(DisplayName = "DefaultMAX") // 발사 타입 최대치
};


UCLASS()
class LETSSHOOT_API AWeapon : public AActor
{
	GENERATED_BODY()

public:
	// 기본 생성자
	AWeapon();

	// 매 프레임마다 호출되는 틱 함수
	virtual void Tick(float DeltaTime) override;

	// 레플리케이션 프로퍼티 설정 함수
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 오너가 레플리케이트 될 때 호출되는 함수
	virtual void OnRep_Owner() override;

	// HUD에 표시될 탄약 설정 함수
	void SetHUDAmmo();

	// 무기 픽업 위젯을 보여줄지 말지 결정하는 함수
	void ShowPickupWidget(bool bShowWidget);

	// 무기 발사 함수. 명중 타겟을 인자로 받음
	virtual void Fire(const FVector& HitTarget);

	// 무기를 드랍하는 함수
	virtual void Dropped();

	// 탄약을 추가하는 함수
	void AddAmmo(int32 AmmoToAdd);

	// 발사 대상까지의 벡터를 계산하는 함수
	FVector TraceEndWithScatter(const FVector& HitTarget);

	// 조준점 텍스처
	UPROPERTY(EditAnywhere, Category = Crosshairs)
		class UTexture2D* CrosshairsCenter;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* CrosshairsLeft;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* CrosshairsRight;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* CrosshairsBottom;

	// 줌 상태에서의 시야각
	UPROPERTY(EditAnywhere)
		float ZoomedFOV = 30.f;

	// 줌 인터폴레이션 속도
	UPROPERTY(EditAnywhere)
		float ZoomInterpSpeed ​​ = 20.f;

	/**
* 자동 발사
*/
	UPROPERTY(EditAnywhere, Category = Combat)
		// 발사 딜레이(발사 간격) 설정 변수
		float FireDelay = .15f;

	UPROPERTY(EditAnywhere, Category = Combat)
		// 자동 발사 활성화 여부 설정 변수
		bool bAutomatic = true;

	UPROPERTY(EditAnywhere)
		// 장착 사운드 설정 변수
		class USoundCue* EquipSound;

	/**
	* 사용자 정의 깊이 활성화 또는 비활성화
	*/
	// 사용자 정의 깊이 설정 함수. 활성화 여부를 인자로 받음
	void EnableCustomDepth(bool bEnable);

	// 무기 파괴 여부 설정 변수
	bool bDestroyWeapon = false;

	UPROPERTY(EditAnywhere)
		// 발사 타입 설정 변수
		EFireType FireType;

	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
		// 발사 시 산발 사용 여부 설정 변수
		bool bUseScatter = false;

protected:
	// 게임 시작 시 실행되는 함수
	virtual void BeginPlay() override;

	// 무기 상태 설정 시 호출되는 함수
	virtual void OnWeaponStateSet();

	// 무기 장착 시 호출되는 함수
	virtual void OnEquipped();

	// 무기 드랍 시 호출되는 함수
	virtual void OnDropped();

	// 무기를 보조 장비로 장착 시 호출되는 함수
	virtual void OnEquippedSecondary();

	// 구체적인 오버랩 이벤트에 대한 함수
	UFUNCTION()
		virtual void OnSphereOverlap(
			UPrimitiveComponent* OverlappedComponent, // 오버랩 된 컴포넌트
			AActor* OtherActor, // 다른 액터
			UPrimitiveComponent* OtherComp, // 다른 컴포넌트
			int32 OtherBodyIndex, // 다른 바디 인덱스
			bool bFromSweep, // 스윕에서 왔는지 여부
			const FHitResult& SweepResult // 스윕 결과
		);

	// 구체적인 오버랩이 끝났을 때의 이벤트에 대한 함수
	UFUNCTION()
		void OnSphereEndOverlap(
			UPrimitiveComponent* OverlappedComponent, // 오버랩 된 컴포넌트
			AActor* OtherActor, // 다른 액터
			UPrimitiveComponent* OtherComp, // 다른 컴포넌트
			int32 OtherBodyIndex // 다른 바디 인덱스
		);

	/**
	* 산발적인 트레이스 종료
	*/

	// 산발 범위까지의 거리 설정 변수
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
		float DistanceToSphere = 800.f;

	// 산발 범위의 반경 설정 변수
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
		float SphereRadius = 75.f;

	// 무기의 기본 피해 설정 변수
	UPROPERTY(EditAnywhere)
		float Damage = 20.f;

	// 헤드샷 시 추가 피해 설정 변수
	UPROPERTY(EditAnywhere)
		float HeadShotDamage = 40.f;

	// 서버 측 리와인드 사용 여부 설정 변수
	UPROPERTY(Replicated, EditAnywhere)
		bool bUseServerSideRewind = false;

	// 블래스터 캐릭터에 대한 참조
	UPROPERTY()
		class ALetsShootCharacter* LetsShootOwnerCharacter;

	// 블래스터 플레이어 컨트롤러에 대한 참조
	UPROPERTY()
		class ALetsShootPlayerController* LetsShootOwnerController;

	// 핑이 너무 높을 때 호출되는 함수
	UFUNCTION()
		void OnPingTooHigh(bool bPingTooHigh);

private:
	// 무기의 메쉬 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
		USkeletalMeshComponent* WeaponMesh;

	// 영역을 표시하는 구체 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
		class USphereComponent* AreaSphere;

	// 무기의 상태를 나타내는 변수
	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
		EWeaponState WeaponState;

	// 무기 상태가 복제되었을 때 호출되는 함수
	UFUNCTION()
		void OnRep_WeaponState();

	// 무기를 줍는 것을 표시하는 위젯 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
		class UWidgetComponent* PickupWidget;

	// 무기 발사 애니메이션
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
		class UAnimationAsset* FireAnimation;

	// 탄피 클래스
	UPROPERTY(EditAnywhere)
		TSubclassOf<class ACasing> CasingClass;

	// 무기에 현재 장착된 탄약 수
	UPROPERTY(EditAnywhere)
		int32 Ammo;

	// 서버의 탄약 상태를 클라이언트에 업데이트하는 함수
	UFUNCTION(Client, Reliable)
		void ClientUpdateAmmo(int32 ServerAmmo);

	// 클라이언트에게 탄약 추가를 지시하는 함수
	UFUNCTION(Client, Reliable)
		void ClientAddAmmo(int32 AmmoToAdd);

	// 한 라운드를 소모하는 함수
	void SpendRound();

	// 무기의 탄창 용량
	UPROPERTY(EditAnywhere)
		int32 MagCapacity;

	// 처리되지 않은 서버의 탄약 요청 수
	// SpendRound에서 증가하고, ClientUpdateAmmo에서 감소합니다.
	int32 Sequence = 0;

	// 무기의 유형
	UPROPERTY(EditAnywhere)
		EWeaponType WeaponType;

	// 무기를 소유한 팀
	UPROPERTY(EditAnywhere)
		ETeam Team;


public:
	// 무기의 상태를 설정하는 함수
	void SetWeaponState(EWeaponState State);

	// 영역 구체를 반환하는 인라인 함수
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; } // 반환되는 영역 구체

	// 무기 메쉬를 반환하는 인라인 함수
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; } // 반환되는 무기 메쉬

	// 픽업 위젯을 반환하는 인라인 함수
	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; } // 반환되는 픽업 위젯

	// 줌된 FOV를 반환하는 인라인 함수
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; } // 반환되는 줌된 FOV

	// 줌 인터폴레이션 속도를 반환하는 인라인 함수
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; } // 반환되는 줌 인터폴레이션 속도

	// 무기의 탄약이 없는지 여부를 확인하는 함수
	bool IsEmpty();

	// 무기의 탄약이 가득 찼는지 여부를 확인하는 함수
	bool IsFull();

	// 무기 타입을 반환하는 인라인 함수
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; } // 반환되는 무기 타입

	// 현재 탄약 수를 반환하는 인라인 함수
	FORCEINLINE int32 GetAmmo() const { return Ammo; } // 반환되는 현재 탄약 수

	// 탄창 용량을 반환하는 인라인 함수
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; } // 반환되는 탄창 용량

	// 무기의 데미지를 반환하는 인라인 함수
	FORCEINLINE float GetDamage() const { return Damage; } // 반환되는 무기 데미지

	// 헤드샷 데미지를 반환하는 인라인 함수
	FORCEINLINE float GetHeadShotDamage() const { return HeadShotDamage; } // 반환되는 헤드샷 데미지

	// 무기를 소유한 팀을 반환하는 인라인 함수
	FORCEINLINE ETeam GetTeam() const { return Team; } // 반환되는 소유 팀

};

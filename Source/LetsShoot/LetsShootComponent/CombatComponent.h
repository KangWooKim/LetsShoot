// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LetsShoot/HUD/LetsShootHUD.h"
#include "LetsShoot/Weapon/WeaponTypes.h"
#include "LetsShoot/LetsShootTypes/CombatState.h"
#include "CombatComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class LETSSHOOT_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// 기본 생성자
	UCombatComponent();

	friend class ALetsShootCharacter;

	// 컴포넌트 틱 함수
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 레플리케이션 속성 설정 함수
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 무기 장착 함수
	// WeaponToEquip : 장착할 무기
	void EquipWeapon(class AWeapon* WeaponToEquip);

	// 무기 교체 함수
	void SwapWeapons();

	// 재장전 함수
	void Reload();

	// 재장전 완료 함수
	UFUNCTION(BlueprintCallable)
		void FinishReloading();

	// 무기 교체 완료 함수
	UFUNCTION(BlueprintCallable)
		void FinishSwap();

	// 무기 교체 과정에서 무기 첨부 완료 함수
	UFUNCTION(BlueprintCallable)
		void FinishSwapAttachWeapons();

	// 발사 버튼이 눌린 상태인지 저장하는 함수
	// bPressed : 버튼이 눌린 상태인지 나타냄
	void FireButtonPressed(bool bPressed);

	// 샷건 탄창 재장전 함수
	UFUNCTION(BlueprintCallable)
		void ShotgunShellReload();

	// 샷건 재장전 종료 함수
	void JumpToShotgunEnd();

	// 수류탄 던지기 완료 함수
	UFUNCTION(BlueprintCallable)
		void ThrowGrenadeFinished();

	// 수류탄 발사 함수
	UFUNCTION(BlueprintCallable)
		void LaunchGrenade();

	// 서버에서 수류탄 발사 함수
	// Target : 수류탄 발사 목표
	UFUNCTION(Server, Reliable)
		void ServerLaunchGrenade(const FVector_NetQuantize& Target);

	// 탄약 획득 함수
	// WeaponType : 무기 유형
	// AmmoAmount : 획득할 탄약 양
	void PickupAmmo(EWeaponType WeaponType, int32 AmmoAmount);

	// 로컬에서 재장전 중인지 나타내는 변수
	bool bLocallyReloading = false;

protected:
	// 게임 플레이가 시작될 때 호출되는 함수
	virtual void BeginPlay() override;

	// 조준 상태 설정 함수
	// bIsAiming : 조준 상태인지 나타냄
	void SetAiming(bool bIsAiming);

	// 서버에서 조준 상태 설정 함수
	// bIsAiming : 조준 상태인지 나타냄
	UFUNCTION(Server, Reliable)
		void ServerSetAiming(bool bIsAiming);

	// 주 장착 무기의 레플리케이션 함수
	UFUNCTION()
		void OnRep_EquippedWeapon();

	// 보조 무기의 레플리케이션 함수
	UFUNCTION()
		void OnRep_SecondaryWeapon();

	// 발사 함수
	void Fire();

	// 발사체 무기 발사 함수
	void FireProjectileWeapon();

	// 히트스캔 무기 발사 함수
	void FireHitScanWeapon();

	// 샷건 발사 함수
	void FireShotgun();

	// 로컬에서의 발사 처리 함수
	// TraceHitTarget : 발사 결과 타겟
	void LocalFire(const FVector_NetQuantize& TraceHitTarget);

	// 로컬에서의 샷건 발사 처리 함수
	// TraceHitTargets : 샷건 발사 결과 타겟들
	void ShotgunLocalFire(const TArray<FVector_NetQuantize>& TraceHitTargets);

	// 서버에서의 발사 처리 함수
	// TraceHitTarget : 발사 결과 타겟
	// FireDelay : 발사 딜레이
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerFire(const FVector_NetQuantize& TraceHitTarget, float FireDelay);

	// 멀티캐스트 발사 함수
	// TraceHitTarget : 발사 결과 타겟
	UFUNCTION(NetMulticast, Reliable)
		void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	// 서버에서의 샷건 발사 처리 함수
	// TraceHitTargets : 샷건 발사 결과 타겟들
	// FireDelay : 발사 딜레이
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets, float FireDelay);

	// 멀티캐스트 샷건 발사 함수
	// TraceHitTargets : 샷건 발사 결과 타겟들
	UFUNCTION(NetMulticast, Reliable)
		void MulticastShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets);

	// 조준점 아래의 히트 정보를 가져오는 함수
	// TraceHitResult : 히트 결과
	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	// HUD 크로스헤어 설정 함수
	// DeltaTime : 델타 시간
	void SetHUDCrosshairs(float DeltaTime);

	// 서버에서의 재장전 함수
	UFUNCTION(Server, Reliable)
		void ServerReload();

	// 재장전 처리 함수
	void HandleReload();

	// 재장전할 탄약량을 반환하는 함수
	int32 AmountToReload();

	// 수류탄 던지기 함수
	void ThrowGrenade();

	// 서버에서 수류탄 던지기 함수
	UFUNCTION(Server, Reliable)
		void ServerThrowGrenade();

	// 수류탄 클래스
	UPROPERTY(EditAnywhere)
		TSubclassOf<class AProjectile> GrenadeClass;

	// 장착중인 무기 버리기 함수
	void DropEquippedWeapon();

	// 오른손에 액터 첨부 함수
	// ActorToAttach : 첨부할 액터
	void AttachActorToRightHand(AActor* ActorToAttach);

	// 왼손에 액터 첨부 함수
	// ActorToAttach : 첨부할 액터
	void AttachActorToLeftHand(AActor* ActorToAttach);

	// 왼손에 깃발 첨부 함수
	// Flag : 첨부할 깃발
	void AttachFlagToLeftHand(AWeapon* Flag);

	// 배낭에 액터 첨부 함수
	// ActorToAttach : 첨부할 액터
	void AttachActorToBackpack(AActor* ActorToAttach);

	// 탄약 업데이트 함수
	void UpdateCarriedAmmo();

	// 무기 장착 소리 재생 함수
	// WeaponToEquip : 장착할 무기
	void PlayEquipWeaponSound(AWeapon* WeaponToEquip);

	// 빈 무기 재장전 함수
	void ReloadEmptyWeapon();

	// 수류탄 보여주기 함수
	// bShowGrenade : 수류탄을 보여줄지 여부
	void ShowAttachedGrenade(bool bShowGrenade);

	// 주 무기 장착 함수
	// WeaponToEquip : 장착할 무기
	void EquipPrimaryWeapon(AWeapon* WeaponToEquip);

	// 보조 무기 장착 함수
	// WeaponToEquip : 장착할 무기
	void EquipSecondaryWeapon(AWeapon* WeaponToEquip);

private:
	// 캐릭터 클래스
	UPROPERTY()
		class ALetsShootCharacter* Character;

	// 플레이어 컨트롤러 클래스
	UPROPERTY()
		class ALetsShootPlayerController* Controller;

	// HUD 클래스
	UPROPERTY()
		class ALetsShootHUD* HUD;

	// 장착된 무기
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
		AWeapon* EquippedWeapon;

	// 보조 무기
	UPROPERTY(ReplicatedUsing = OnRep_SecondaryWeapon)
		AWeapon* SecondaryWeapon;

	// 조준중 여부
	UPROPERTY(ReplicatedUsing = OnRep_Aiming)
		bool bAiming = false;

	// 조준 버튼 눌림 여부
	bool bAimButtonPressed = false;

	// 조준 여부의 레플리케이션 함수
	UFUNCTION()
		void OnRep_Aiming();

	// 기본 걷기 속도
	UPROPERTY(EditAnywhere)
		float BaseWalkSpeed;

	// 조준시 걷기 속도
	UPROPERTY(EditAnywhere)
		float AimWalkSpeed;

	// 발사 버튼 눌림 여부
	bool bFireButtonPressed;

	/**
	* HUD와 크로스헤어
	*/

	// 크로스헤어의 속도 요소
	float CrosshairVelocityFactor;

	// 공중에 있는 동안의 크로스헤어 요소
	float CrosshairInAirFactor;

	// 조준하는 동안의 크로스헤어 요소
	float CrosshairAimFactor;

	// 발사하는 동안의 크로스헤어 요소
	float CrosshairShootingFactor;

	// 히트 타겟 위치
	FVector HitTarget;

	// HUD 패키지
	FHUDPackage HUDPackage;

	/**
	* 조준 및 시야각(FOV)
	*/

	// 조준하지 않는 동안의 시야각; BeginPlay에서 카메라의 기본 FOV로 설정됨
	float DefaultFOV;

	// 조준 시의 시야각
	UPROPERTY(EditAnywhere, Category = Combat)
		float ZoomedFOV = 30.f;

	// 현재 시야각
	float CurrentFOV;

	// 시야각 변경 속도
	UPROPERTY(EditAnywhere, Category = Combat)
		float ZoomInterpSpeed = 20.f;

	// 시야각 보간 함수
	// DeltaTime : 델타 시간
	void InterpFOV(float DeltaTime);

	/**
	* 자동 발사
	*/

	// 발사 타이머
	FTimerHandle FireTimer;

	// 발사 가능 여부
	bool bCanFire = true;

	// 발사 타이머 시작 함수
	void StartFireTimer();

	// 발사 타이머 종료 함수
	void FireTimerFinished();

	// 발사 가능 여부 반환 함수
	bool CanFire();

	// 현재 장착된 무기에 대한 보유 탄약량
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
		int32 CarriedAmmo;

	// 보유 탄약량의 레플리케이션 함수
	UFUNCTION()
		void OnRep_CarriedAmmo();

	// 무기 유형에 따른 보유 탄약량 맵
	TMap<EWeaponType, int32> CarriedAmmoMap;

	// 최대 보유 가능 탄약량
	UPROPERTY(EditAnywhere)
		int32 MaxCarriedAmmo = 500;

	// 시작 시 AR 탄약량
	UPROPERTY(EditAnywhere)
		int32 StartingARAmmo = 30;

	// 시작 시 로켓 탄약량
	UPROPERTY(EditAnywhere)
		int32 StartingRocketAmmo = 0;

	// 시작 시 피스톨 탄약량
	UPROPERTY(EditAnywhere)
		int32 StartingPistolAmmo = 0;

	// 시작 시 SMG 탄약량
	UPROPERTY(EditAnywhere)
		int32 StartingSMGAmmo = 0;

	// 시작 시 샷건 탄약량
	UPROPERTY(EditAnywhere)
		int32 StartingShotgunAmmo = 0;

	// 시작 시 스나이퍼 탄약량
	UPROPERTY(EditAnywhere)
		int32 StartingSniperAmmo = 0;

	// 시작 시 수류탄 발사기 탄약량
	UPROPERTY(EditAnywhere)
		int32 StartingGrenadeLauncherAmmo = 0;


	// 보유 탄약 초기화 함수
	void InitializeCarriedAmmo();

	// 전투 상태 변수
	// ECombatState::ECS_Unoccupied로 초기화
	// 전투 상태의 변화를 추적하고 대응하기 위해 사용
	// 레플리케이션 속성을 가지고 있어, 네트워크 상의 다른 액터에게 전투 상태를 동기화
	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
		ECombatState CombatState = ECombatState::ECS_Unoccupied;

	// 전투 상태가 변경될 때마다 호출되는 함수
	UFUNCTION()
		void OnRep_CombatState();

	// 보유 탄약 수치 업데이트 함수
	void UpdateAmmoValues();

	// 샷건 탄약 수치 업데이트 함수
	void UpdateShotgunAmmoValues();

	// 수류탄 수 변수, 초기 값은 4
	// 수류탄의 개수를 추적하고 대응하기 위해 사용
	// 레플리케이션 속성을 가지고 있어, 네트워크 상의 다른 액터에게 수류탄 개수를 동기화
	UPROPERTY(ReplicatedUsing = OnRep_Grenades)
		int32 Grenades = 4;

	// 수류탄 개수가 변경될 때마다 호출되는 함수
	UFUNCTION()
		void OnRep_Grenades();

	// 최대 수류탄 개수, 기본값은 4
	UPROPERTY(EditAnywhere)
		int32 MaxGrenades = 4;

	// HUD의 수류탄 개수 업데이트 함수
	void UpdateHUDGrenades();

	// 플래그 보유 여부 변수
	// 플래그 보유 상태를 추적하고 대응하기 위해 사용
	// 레플리케이션 속성을 가지고 있어, 네트워크 상의 다른 액터에게 플래그 보유 상태를 동기화
	UPROPERTY(ReplicatedUsing = OnRep_HoldingTheFlag)
		bool bHoldingTheFlag = false;

	// 플래그 보유 여부가 변경될 때마다 호출되는 함수
	UFUNCTION()
		void OnRep_HoldingTheFlag();

	// 보유 중인 플래그 객체
	UPROPERTY()
		AWeapon* TheFlag;

public:
	// 수류탄 개수를 반환하는 함수
	FORCEINLINE int32 GetGrenades() const { return Grenades; }

	// 무기를 교체해야 하는지를 결정하는 함수
	bool ShouldSwapWeapons();

};

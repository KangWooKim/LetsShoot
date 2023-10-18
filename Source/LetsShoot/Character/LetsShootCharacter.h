// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LetsShoot/LetsShootTypes/TurningInPlace.h"
#include "LetsShoot/Interfaces/InteractWithCrosshairsInterface.h"
#include "Components/TimelineComponent.h"
#include "LetsShoot/LetsShootTypes/CombatState.h"
#include "LetsShoot/LetsShootTypes/Team.h"
#include "LetsShootCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeftGame);

UCLASS()
class LETSSHOOT_API ALetsShootCharacter : public ACharacter, public ICrossHairInterface
{
	GENERATED_BODY()

public:
	ALetsShootCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

	/**
	 * 블래스터 캐릭터 클래스의 생성자입니다.
	 */
	ALetsShootCharacter();

	/**
	 * 매 프레임마다 실행되는 로직을 담당하는 함수입니다.
	 */
	virtual void Tick(float DeltaTime) override;

	/**
	 * 플레이어 입력 설정을 담당하는 함수입니다.
	 */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/**
	 * 네트워크 레플리케이션을 위한 속성 설정을 담당하는 함수입니다.
	 */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * 컴포넌트 초기화 후에 호출되는 함수입니다.
	 */
	virtual void PostInitializeComponents() override;

	/**
	 * 발사 몽타주를 재생하는 함수입니다.
	 * @param bAiming 조준 중인지 여부를 나타냅니다.
	 */
	void PlayFireMontage(bool bAiming);

	/**
	 * 재장전 몽타주를 재생하는 함수입니다.
	 */
	void PlayReloadMontage();

	/**
	 * 제거 몽타주를 재생하는 함수입니다.
	 */
	void PlayElimMontage();

	/**
	 * 수류탄 던지기 몽타주를 재생하는 함수입니다.
	 */
	void PlayThrowGrenadeMontage();

	/**
	 * 무기 교체 몽타주를 재생하는 함수입니다.
	 */
	void PlaySwapMontage();

	/**
	 * ReplicatedMovement가 변경될 때 호출되는 함수입니다.
	 */
	virtual void OnRep_ReplicatedMovement() override;

	/**
	 * 플레이어를 제거하는 함수입니다.
	 * @param bPlayerLeftGame 플레이어가 게임을 나갔는지 여부를 나타냅니다.
	 */
	void Elim(bool bPlayerLeftGame);

	/**
	 * 플레이어를 제거하는 함수를 멀티캐스트로 실행하는 함수입니다.
	 * @param bPlayerLeftGame 플레이어가 게임을 나갔는지 여부를 나타냅니다.
	 */
	UFUNCTION(NetMulticast, Reliable)
		void MulticastElim(bool bPlayerLeftGame);

	/**
	 * 객체가 제거될 때 호출되는 함수입니다.
	 */
	virtual void Destroyed() override;

	/**
	 * 게임플레이 비활성화 상태를 나타내는 변수입니다.
	 */
	UPROPERTY(Replicated)
		bool bDisableGameplay = false;

	/**
	 * 스나이퍼 스코프 위젯을 보여주는 이벤트입니다.
	 * @param bShowScope 스나이퍼 스코프 위젯을 보여줄지 여부를 나타냅니다.
	 */
	UFUNCTION(BlueprintImplementableEvent)
		void ShowSniperScopeWidget(bool bShowScope);

	/**
	 * HUD의 체력을 업데이트하는 함수입니다.
	 */
	void UpdateHUDHealth();

	/**
	 * HUD의 방패를 업데이트하는 함수입니다.
	 */
	void UpdateHUDShield();

	/**
	 * HUD의 탄약을 업데이트하는 함수입니다.
	 */
	void UpdateHUDAmmo();

	/**
	 * 기본 무기를 생성하는 함수입니다.
	 */
	void SpawDefaultWeapon();

	/**
	 * 히트 박스를 저장하는 맵입니다.
	 */
	UPROPERTY()
		TMap<FName, class UBoxComponent*> HitCollisionBoxes;

	/**
	 * 무기 교체를 완료했는지 여부를 나타내는 변수입니다.
	 */
	bool bFinishedSwapping = false;

	/**
	 * 게임에서 나가는 함수를 서버에 호출하는 함수입니다.
	 */
	UFUNCTION(Server, Reliable)
		void ServerLeaveGame();

	/**
	 * 게임에서 나갔을 때 발생하는 델리게이트입니다.
	 */
	FOnLeftGame OnLeftGame;

	/**
	 * 리더가 되었을 때 호출되는 멀티캐스트 함수입니다.
	 */
	UFUNCTION(NetMulticast, Reliable)
		void MulticastGainedTheLead();

	/**
	 * 리더에서 해제되었을 때 호출되는 멀티캐스트 함수입니다.
	 */
	UFUNCTION(NetMulticast, Reliable)
		void MulticastLostTheLead();

	/**
	 * 팀 색상을 설정하는 함수입니다.
	 * @param Team 팀을 나타내는 열거형 변수입니다.
	 */
	void SetTeamColor(ETeam Team);


protected:
	/**
	 * 게임 시작 시 호출되는 함수입니다.
	 */
	virtual void BeginPlay() override;

	/**
	 * 캐릭터를 앞으로 이동시키는 함수입니다.
	 * @param Value 이동 값입니다.
	 */
	void MoveForward(float Value);

	/**
	 * 캐릭터를 오른쪽으로 이동시키는 함수입니다.
	 * @param Value 이동 값입니다.
	 */
	void MoveRight(float Value);

	/**
	 * 캐릭터의 좌우 회전을 처리하는 함수입니다.
	 * @param Value 회전 값입니다.
	 */
	void Turn(float Value);

	/**
	 * 캐릭터의 상하 시선을 처리하는 함수입니다.
	 * @param Value 시선 값입니다.
	 */
	void LookUp(float Value);

	/**
	 * 무기 장착 버튼을 눌렀을 때 호출되는 함수입니다.
	 */
	void EquipButtonPressed();

	/**
	 * 캐릭터 앉기 버튼을 눌렀을 때 호출되는 함수입니다.
	 */
	void CrouchButtonPressed();

	/**
	 * 재장전 버튼을 눌렀을 때 호출되는 함수입니다.
	 */
	void ReloadButtonPressed();

	/**
	 * 조준 버튼을 눌렀을 때 호출되는 함수입니다.
	 */
	void AimButtonPressed();

	/**
	 * 조준 버튼을 놓았을 때 호출되는 함수입니다.
	 */
	void AimButtonReleased();

	/**
	 * 시간 경과에 따른 조준 위치 보정을 처리하는 함수입니다.
	 * @param DeltaTime 이전 프레임과의 시간 차이입니다.
	 */
	void AimOffset(float DeltaTime);

	/**
	 * AO_Pitch 값을 계산하는 함수입니다.
	 */
	void CalculateAO_Pitch();

	/**
	 * SimProxy 턴을 처리하는 함수입니다.
	 */
	void SimProxiesTurn();

	/**
	 * 점프를 처리하는 함수입니다.
	 */
	virtual void Jump() override;

	/**
	 * 발사 버튼을 눌렀을 때 호출되는 함수입니다.
	 */
	void FireButtonPressed();

	/**
	 * 발사 버튼을 놓았을 때 호출되는 함수입니다.
	 */
	void FireButtonReleased();

	/**
	 * 피격 리액션 몽타주를 재생하는 함수입니다.
	 */
	void PlayHitReactMontage();

	/**
	 * 수류탄 버튼을 눌렀을 때 호출되는 함수입니다.
	 */
	void GrenadeButtonPressed();

	/**
	 * 무기를 드롭하거나 파괴하는 함수입니다.
	 * @param Weapon 드롭하거나 파괴할 무기입니다.
	 */
	void DropOrDestroyWeapon(AWeapon* Weapon);

	/**
	 * 모든 무기를 드롭하거나 파괴하는 함수입니다.
	 */
	void DropOrDestroyWeapons();

	/**
	 * 스폰 지점을 설정하는 함수입니다.
	 */
	void SetSpawnPoint();

	/**
	 * 플레이어 상태가 초기화되었을 때 호출되는 함수입니다.
	 */
	void OnPlayerStateInitialized();

	/**
	 * 데미지를 받았을 때 호출되는 함수입니다.
	 * @param DamagedActor 피해를 받은 액터입니다.
	 * @param Damage 받은 데미지 값입니다.
	 * @param DamageType 데미지 유형입니다.
	 * @param InstigatorController 피해를 가한 컨트롤러입니다.
	 * @param DamageCauser 피해를 가한 액터입니다.
	 */
	UFUNCTION()
		void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);

	/**
	 * 필요한 클래스를 확인하고 HUD를 초기화하는 함수입니다.
	 */
	void PollInit();

	/**
	 * 플레이어가 제자리에서 회전하는 것을 처리하는 함수입니다.
	 * @param DeltaTime 이전 프레임과의 시간 차이입니다.
	 */
	void RotateInPlace(float DeltaTime);

	/**
	 * 서버 사이드 리와인드에 사용되는 히트 박스입니다.
	 */
	UPROPERTY(EditAnywhere)
		class UBoxComponent* head;

	UPROPERTY(EditAnywhere)
		UBoxComponent* pelvis;

	UPROPERTY(EditAnywhere)
		UBoxComponent* spine_02;

	UPROPERTY(EditAnywhere)
		UBoxComponent* spine_03;

	UPROPERTY(EditAnywhere)
		UBoxComponent* upperarm_l;

	UPROPERTY(EditAnywhere)
		UBoxComponent* upperarm_r;

	UPROPERTY(EditAnywhere)
		UBoxComponent* lowerarm_l;

	UPROPERTY(EditAnywhere)
		UBoxComponent* lowerarm_r;

	UPROPERTY(EditAnywhere)
		UBoxComponent* hand_l;

	UPROPERTY(EditAnywhere)
		UBoxComponent* hand_r;

	UPROPERTY(EditAnywhere)
		UBoxComponent* backpack;

	UPROPERTY(EditAnywhere)
		UBoxComponent* blanket;

	UPROPERTY(EditAnywhere)
		UBoxComponent* thigh_l;

	UPROPERTY(EditAnywhere)
		UBoxComponent* thigh_r;

	UPROPERTY(EditAnywhere)
		UBoxComponent* calf_l;

	UPROPERTY(EditAnywhere)
		UBoxComponent* calf_r;

	UPROPERTY(EditAnywhere)
		UBoxComponent* foot_l;

	UPROPERTY(EditAnywhere)
		UBoxComponent* foot_r;



private:
	/**
	 * 카메라의 스프링 암 컴포넌트입니다.
	 */
	UPROPERTY(VisibleAnywhere, Category = Camera)
		class USpringArmComponent* CameraBoom;

	/**
	 * 카메라 컴포넌트입니다.
	 */
	UPROPERTY(VisibleAnywhere, Category = Camera)
		class UCameraComponent* FollowCamera;

	/**
	 * 캐릭터의 위에 표시되는 위젯 컴포넌트입니다.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* OverheadWidget;

	/**
	 * 겹쳐진 무기를 나타내는 변수입니다.
	 */
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
		class AWeapon* OverlappingWeapon;

	/**
	 * 겹쳐진 무기가 변경되었을 때 호출되는 함수입니다.
	 * @param LastWeapon 이전의 무기입니다.
	 */
	UFUNCTION()
		void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	/**
	 * LetsShoot 컴포넌트들입니다.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UCombatComponent* Combat;

	UPROPERTY(VisibleAnywhere)
		class UBuffComponent* Buff;

	UPROPERTY(VisibleAnywhere)
		class ULagCompensationComponent* LagCompensation;

	/**
	 * AO_Yaw, InterpAO_Yaw, AO_Pitch, StartingAimRotation에 대한 변수입니다.
	 */
	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	/**
	 * InPlace 턴을 처리하는 함수입니다.
	 * @param DeltaTime 이전 프레임과의 시간 차이입니다.
	 */
	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);

	/**
	 * 애니메이션 몽타주들입니다.
	 */
	UPROPERTY(EditAnywhere, Category = Combat)
		class UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
		UAnimMontage* ReloadMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
		UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
		UAnimMontage* ElimMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
		UAnimMontage* ThrowGrenadeMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
		UAnimMontage* SwapMontage;

	/**
	 * 캐릭터가 가까이 있을 때 카메라를 숨기는 함수입니다.
	 */
	void HideCameraIfCharacterClose();

	/**
	 * 캐릭터와 카메라의 거리 임계값입니다.
	 */
	UPROPERTY(EditAnywhere)
		float CameraThreshold = 200.f;

	bool bRotateRootBone;

	/**
	 * 턴 임계값입니다.
	 */
	float TurnThreshold = 0.5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYaw;
	float TimeSinceLastMovementReplication;
	float CalculateSpeed();

	/**
	 * 플레이어의 체력 관련 변수들입니다.
	 */
	UPROPERTY(EditAnywhere, Category = "Player Stats")
		float MaxHealth = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
		float Health = 100.f;

	/**
	 * 체력이 변경되었을 때 호출되는 함수입니다.
	 * @param LastHealth 이전의 체력 값입니다.
	 */
	UFUNCTION()
		void OnRep_Health(float LastHealth);

	/**
	 * 플레이어의 실드 관련 변수들입니다.
	 */
	UPROPERTY(EditAnywhere, Category = "Player Stats")
		float MaxShield = 100.f;

	UPROPERTY(ReplicatedUsing = OnRep_Shield, EditAnywhere, Category = "Player Stats")
		float Shield = 0.f;

	/**
	 * 실드가 변경되었을 때 호출되는 함수입니다.
	 * @param LastShield 이전의 실드 값입니다.
	 */
	UFUNCTION()
		void OnRep_Shield(float LastShield);

	/**
	 * LetsShoot 플레이어 컨트롤러입니다.
	 */
	UPROPERTY()
		class ALetsShootPlayerController* LetsShootPlayerController;

	/**
	 * 플레이어가 제거되었는지를 나타내는 변수입니다.
	 */
	bool bElimmed = false;

	FTimerHandle ElimTimer;

	/**
	 * 제거 딜레이 시간입니다.
	 */
	UPROPERTY(EditDefaultsOnly)
		float ElimDelay = 3.f;

	/**
	 * 제거 타이머가 완료되었을 때 호출되는 함수입니다.
	 */
	void ElimTimerFinished();

	/**
	 * 플레이어가 게임에서 나갔는지를 나타내는 변수입니다.
	 */
	bool bLeftGame = false;



/**
 * 디졸브 효과와 관련된 변수들입니다.
 */

 // 디졸브 효과에 사용되는 타임라인 컴포넌트입니다.
	UPROPERTY(VisibleAnywhere)
		UTimelineComponent* DissolveTimeline;

	// 디졸브 효과를 업데이트하는 데 사용되는 함수입니다.
	UFUNCTION()
		void UpdateDissolveMaterial(float DissolveValue);

	// 디졸브 효과를 시작하는 함수입니다.
	void StartDissolve();

	// 런타임에서 변경할 수 있는 동적 인스턴스입니다.
	UPROPERTY(VisibleAnywhere, Category = Elim)
		UMaterialInstanceDynamic* DynamicDissolveMaterialInstance;

	// 블루프린트에서 설정한 소거(Material Dissolve) 머티리얼 인스턴스입니다.
	UPROPERTY(VisibleAnywhere, Category = Elim)
		UMaterialInstance* DissolveMaterialInstance;

	/**
	 * 팀 색상과 관련된 변수들입니다.
	 */

	 // 빨간 팀의 소거(Material Dissolve) 머티리얼 인스턴스입니다.
	UPROPERTY(EditAnywhere, Category = Elim)
		UMaterialInstance* RedDissolveMatInst;

	// 빨간 팀의 머티리얼 인스턴스입니다.
	UPROPERTY(EditAnywhere, Category = Elim)
		UMaterialInstance* RedMaterial;

	// 파란 팀의 소거(Material Dissolve) 머티리얼 인스턴스입니다.
	UPROPERTY(EditAnywhere, Category = Elim)
		UMaterialInstance* BlueDissolveMatInst;

	// 파란 팀의 머티리얼 인스턴스입니다.
	UPROPERTY(EditAnywhere, Category = Elim)
		UMaterialInstance* BlueMaterial;

	// 원본 머티리얼 인스턴스입니다.
	UPROPERTY(EditAnywhere, Category = Elim)
		UMaterialInstance* OriginalMaterial;

	/**
	 * 제거 효과와 관련된 변수들입니다.
	 */

	 // 제거 로봇 효과에 사용되는 파티클 시스템입니다.
	UPROPERTY(EditAnywhere)
		UParticleSystem* ElimBotEffect;

	// 제거 로봇 효과에 사용되는 파티클 시스템 컴포넌트입니다.
	UPROPERTY(VisibleAnywhere)
		UParticleSystemComponent* ElimBotComponent;

	// 제거 로봇 효과에 사용되는 사운드 큐입니다.
	UPROPERTY(EditAnywhere)
		class USoundCue* ElimBotSound;

	// LetsShoot 플레이어 상태입니다.
	UPROPERTY()
		class ALetsShootPlayerState* LetsShootPlayerState;

	// 왕관 효과에 사용되는 Niagara 시스템입니다.
	UPROPERTY(EditAnywhere)
		class UNiagaraSystem* CrownSystem;

	// 왕관 효과에 사용되는 Niagara 컴포넌트입니다.
	UPROPERTY()
		class UNiagaraComponent* CrownComponent;

	/**
	 * 수류탄과 관련된 변수입니다.
	 */

	 // 캐릭터에 부착된 수류탄의 정적 메시 컴포넌트입니다.
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* AttachedGrenade;

	/**
	 * 기본 무기와 관련된 변수들입니다.
	 */

	 // 기본 무기 클래스입니다.
	UPROPERTY(EditAnywhere)
		TSubclassOf<AWeapon> DefaultWeaponClass;

	// LetsShoot 게임 모드입니다.
	UPROPERTY()
		class ALetsShootGameMode* LetsShootGameMode;



public:
	/**
	 * 현재 겹치는 무기를 설정하는 함수입니다.
	 */
	void SetOverlappingWeapon(AWeapon* Weapon);

	/**
	 * 현재 무기를 장착 중인지 확인하는 함수입니다.
	 * @return 무기를 장착 중인지 여부
	 */
	bool IsWeaponEquipped();

	/**
	 * 현재 조준 중인지 확인하는 함수입니다.
	 * @return 조준 중인지 여부
	 */
	bool IsAiming();

	/**
	 * Yaw에 대한 Aim Offset 값을 반환하는 함수입니다.
	 * @return Yaw에 대한 Aim Offset 값
	 */
	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }

	/**
	 * Pitch에 대한 Aim Offset 값을 반환하는 함수입니다.
	 * @return Pitch에 대한 Aim Offset 값
	 */
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }

	/**
	 * 현재 장착된 무기를 반환하는 함수입니다.
	 * @return 장착된 무기
	 */
	AWeapon* GetEquippedWeapon();

	/**
	 * 회전 중인 상태(턴 중인 상태)를 반환하는 함수입니다.
	 * @return 회전 중인 상태(턴 중인 상태)
	 */
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }

	/**
	 * 공격 대상 지점을 반환하는 함수입니다.
	 * @return 공격 대상 지점
	 */
	FVector GetHitTarget() const;

	/**
	 * Follow 카메라 컴포넌트를 반환하는 함수입니다.
	 * @return Follow 카메라 컴포넌트
	 */
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/**
	 * Root Bone 회전 여부를 반환하는 함수입니다.
	 * @return Root Bone을 회전해야 하는지 여부
	 */
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }

	/**
	 * 제거된 상태인지 확인하는 함수입니다.
	 * @return 제거된 상태인지 여부
	 */
	FORCEINLINE bool IsElimmed() const { return bElimmed; }

	/**
	 * 현재 체력을 반환하는 함수입니다.
	 * @return 현재 체력
	 */
	FORCEINLINE float GetHealth() const { return Health; }

	/**
	 * 현재 체력을 설정하는 함수입니다.
	 * @param Amount 설정할 체력 값
	 */
	FORCEINLINE void SetHealth(float Amount) { Health = Amount; }

	/**
	 * 최대 체력을 반환하는 함수입니다.
	 * @return 최대 체력
	 */
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	/**
	 * 현재 실드 값을 반환하는 함수입니다.
	 * @return 현재 실드 값
	 */
	FORCEINLINE float GetShield() const { return Shield; }

	/**
	 * 현재 실드 값을 설정하는 함수입니다.
	 * @param Amount 설정할 실드 값
	 */
	FORCEINLINE void SetShield(float Amount) { Shield = Amount; }

	/**
	 * 최대 실드 값을 반환하는 함수입니다.
	 * @return 최대 실드 값
	 */
	FORCEINLINE float GetMaxShield() const { return MaxShield; }

	/**
	 * 전투 상태를 반환하는 함수입니다.
	 * @return 전투 상태
	 */
	ECombatState GetCombatState() const;

	/**
	 * 전투 컴포넌트를 반환하는 함수입니다.
	 * @return 전투 컴포넌트
	 */
	FORCEINLINE UCombatComponent* GetCombat() const { return Combat; }

	/**
	 * 게임 플레이 비활성화 여부를 반환하는 함수입니다.
	 * @return 게임 플레이 비활성화 여부
	 */
	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }

	/**
	 * 리로딩 중인지 확인하는 함수입니다.
	 * @return 리로딩 중인지 여부
	 */
	bool IsLocallyReloading();

	/**
	 * 랙 보상 컴포넌트를 반환하는 함수입니다.
	 * @return 랙 보상 컴포넌트
	 */
	FORCEINLINE ULagCompensationComponent* GetLagCompensation() const { return LagCompensation; }

	/**
	 * 깃발을 들고 있는지 확인하는 함수입니다.
	 * @return 깃발을 들고 있는지 여부
	 */
	FORCEINLINE bool IsHoldingTheFlag() const;

	/**
	 * 플레이어의 팀을 반환하는 함수입니다.
	 * @return 플레이어의 팀
	 */
	ETeam GetTeam();

	/**
	 * 깃발을 들고 있는지 여부를 설정하는 함수입니다.
	 * @param bHolding 깃발을 들고 있는지 여부
	 */
	void SetHoldingTheFlag(bool bHolding);

};

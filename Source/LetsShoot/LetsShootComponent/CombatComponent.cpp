// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "LetsShoot/Weapon/Weapon.h"
#include "LetsShoot/Character/LetsShootCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "LetsShoot/PlayerController/LetsShootPlayerController.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"
#include "LetsShoot/Character/LetsShootAnimInstance.h"
#include "LetsShoot/Weapon/Projectile.h"
#include "LetsShoot/Weapon/Shotgun.h"

// UCombatComponent 클래스 생성자
UCombatComponent::UCombatComponent()
{
	// 이 컴포넌트가 틱 함수를 수행하도록 설정
	PrimaryComponentTick.bCanEverTick = true;

	// 기본 걷기 속도와 조준 걷기 속도 초기화
	BaseWalkSpeed = 600.f;
	AimWalkSpeed = 450.f;
}

// 복제되어야 할 속성을 지정하는 함수 (복제되는 속성: 장착된 무기, 보조 무기, 조준 중 여부, 가진 탄약, 전투 상태, 수류탄, 깃발 보유 여부)
void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, SecondaryWeapon);
	DOREPLIFETIME(UCombatComponent, bAiming);
	DOREPLIFETIME_CONDITION(UCombatComponent, CarriedAmmo, COND_OwnerOnly);
	DOREPLIFETIME(UCombatComponent, CombatState);
	DOREPLIFETIME(UCombatComponent, Grenades);
	DOREPLIFETIME(UCombatComponent, bHoldingTheFlag);
}

// 샷건 탄약을 업데이트하는 메서드
void UCombatComponent::ShotgunShellReload()
{
	// 캐릭터가 존재하고, 권한이 있다면 샷건 탄약 값을 업데이트
	if (Character && Character->HasAuthority())
	{
		UpdateShotgunAmmoValues();
	}

}

// 탄약 획득 메서드 (WeaponType: 무기 타입, AmmoAmount: 획득할 탄약 수)
void UCombatComponent::PickupAmmo(EWeaponType WeaponType, int32 AmmoAmount)
{
	// 획득한 무기 타입의 탄약이 가진 탄약 맵에 있을 경우
	if (CarriedAmmoMap.Contains(WeaponType))
	{
		// 가진 탄약을 추가하고, 최대 가질 수 있는 탄약 수를 넘지 않도록 조절 후, 탄약 정보 업데이트
		CarriedAmmoMap[WeaponType] = FMath::Clamp(CarriedAmmoMap[WeaponType] + AmmoAmount, 0, MaxCarriedAmmo);
		UpdateCarriedAmmo();
	}
	// 장착한 무기가 없거나, 무기가 비어 있고, 획득한 무기 타입과 장착한 무기의 타입이 같을 경우
	if (EquippedWeapon && EquippedWeapon->IsEmpty() && EquippedWeapon->GetWeaponType() == WeaponType)
	{
		// 재장전
		Reload();
	}
}

// 플레이 시작 시 호출되는 메서드
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	// 캐릭터가 존재하는 경우
	if (Character)
	{
		// 캐릭터의 최대 걷기 속도를 기본 걷기 속도로 설정
		Character->GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

		// 캐릭터가 카메라를 따라가는 경우
		if (Character->GetFollowCamera())
		{
			// 기본 시야 각도와 현재 시야 각도 설정
			DefaultFOV = Character->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
		// 캐릭터가 권한을 가진 경우
		if (Character->HasAuthority())
		{
			// 가진 탄약 초기화
			InitializeCarriedAmmo();
		}
	}
}

// 틱 함수 (DeltaTime: 이전 프레임 이후의 시간, TickType: 틱 유형, ThisTickFunction: 이 틱 함수)
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 캐릭터가 존재하고, 로컬에서 제어되는 경우
	if (Character && Character->IsLocallyControlled())
	{
		// 교차점 아래 추적
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		HitTarget = HitResult.ImpactPoint;

		// HUD 십자선 설정 및 시야 각도 보간
		SetHUDCrosshairs(DeltaTime);
		InterpFOV(DeltaTime);
	}
}


// 발사 버튼이 눌렸는지 확인하는 메서드 (bPressed: 발사 버튼이 눌렸는지 여부)
void UCombatComponent::FireButtonPressed(bool bPressed)
{
	// 발사 버튼이 눌렸는지 여부를 업데이트
	bFireButtonPressed = bPressed;
	// 발사 버튼이 눌린 경우, 발사 메서드를 호출
	if (bFireButtonPressed)
	{
		Fire();
	}
}

// 무기를 발사하는 메서드
void UCombatComponent::Fire()
{
	// 발사 가능한 경우에만 발사
	if (CanFire())
	{
		bCanFire = false;
		if (EquippedWeapon)
		{
			CrosshairShootingFactor = .75f;

			// 장착된 무기의 발사 유형에 따라 적절한 발사 메서드를 호출
			switch (EquippedWeapon->FireType)
			{
			case EFireType::EFT_Projectile:
				FireProjectileWeapon();
				break;
			case EFireType::EFT_HitScan:
				FireHitScanWeapon();
				break;
			case EFireType::EFT_Shotgun:
				FireShotgun();
				break;
			}
		}
		// 발사 타이머 시작
		StartFireTimer();
	}
}

// 발사체 무기를 발사하는 메서드
void UCombatComponent::FireProjectileWeapon()
{
	if (EquippedWeapon && Character)
	{
		// Scatter 옵션에 따라 타격 위치를 계산
		HitTarget = EquippedWeapon->bUseScatter ? EquippedWeapon->TraceEndWithScatter(HitTarget) : HitTarget;
		if (!Character->HasAuthority()) LocalFire(HitTarget);
		// 서버에서 발사 처리
		ServerFire(HitTarget, EquippedWeapon->FireDelay);
	}
}

// 히트스캔 무기를 발사하는 메서드
void UCombatComponent::FireHitScanWeapon()
{
	if (EquippedWeapon && Character)
	{
		// Scatter 옵션에 따라 타격 위치를 계산
		HitTarget = EquippedWeapon->bUseScatter ? EquippedWeapon->TraceEndWithScatter(HitTarget) : HitTarget;
		if (!Character->HasAuthority()) LocalFire(HitTarget);
		// 서버에서 발사 처리
		ServerFire(HitTarget, EquippedWeapon->FireDelay);
	}
}

// 샷건을 발사하는 메서드
void UCombatComponent::FireShotgun()
{
	AShotgun* Shotgun = Cast<AShotgun>(EquippedWeapon);
	if (Shotgun && Character)
	{
		TArray<FVector_NetQuantize> HitTargets;
		// 샷건이 여러 타겟에 타격할 수 있도록 처리
		Shotgun->ShotgunTraceEndWithScatter(HitTarget, HitTargets);
		if (!Character->HasAuthority()) ShotgunLocalFire(HitTargets);
		// 서버에서 발사 처리
		ServerShotgunFire(HitTargets, EquippedWeapon->FireDelay);
	}
}

// 발사 타이머를 시작하는 메서드
void UCombatComponent::StartFireTimer()
{
	if (EquippedWeapon == nullptr || Character == nullptr) return;
	// 발사 간격 동안 타이머를 설정하고, 타이머가 끝나면 FireTimerFinished 메서드 호출
	Character->GetWorldTimerManager().SetTimer(
		FireTimer,
		this,
		&UCombatComponent::FireTimerFinished,
		EquippedWeapon->FireDelay
	);
}
// 발사 타이머가 끝났을 때의 처리를 하는 메서드
void UCombatComponent::FireTimerFinished()
{
	if (EquippedWeapon == nullptr) return;
	bCanFire = true;
	// 발사 버튼이 계속 눌려져 있고, 무기가 자동 발사형인 경우 다시 발사
	if (bFireButtonPressed && EquippedWeapon->bAutomatic)
	{
		Fire();
	}
	// 무기가 비어있으면 재장전
	ReloadEmptyWeapon();
}

// 서버에서 발사를 처리하는 메서드 (TraceHitTarget: 타격 위치, FireDelay: 발사 딜레이)
void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget, float FireDelay)
{
	// 모든 클라이언트에게 발사를 알림
	MulticastFire(TraceHitTarget);
}

// 서버에서 발사를 검증하는 메서드 (TraceHitTarget: 타격 위치, FireDelay: 발사 딜레이)
bool UCombatComponent::ServerFire_Validate(const FVector_NetQuantize& TraceHitTarget, float FireDelay)
{
	if (EquippedWeapon)
	{
		// 무기의 발사 딜레이와 파라미터의 발사 딜레이가 거의 같은지 검증
		bool bNearlyEqual = FMath::IsNearlyEqual(EquippedWeapon->FireDelay, FireDelay, 0.001f);
		return bNearlyEqual;
	}
	return true;
}

// 모든 클라이언트에게 발사를 알리는 메서드 (TraceHitTarget: 타격 위치)
void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	// 로컬 컨트롤이고 권한이 없는 클라이언트는 리턴
	if (Character && Character->IsLocallyControlled() && !Character->HasAuthority()) return;
	LocalFire(TraceHitTarget);
}

// 서버에서 샷건 발사를 처리하는 메서드 (TraceHitTargets: 샷건 타격 위치들, FireDelay: 발사 딜레이)
void UCombatComponent::ServerShotgunFire_Implementation(const TArray<FVector_NetQuantize>& TraceHitTargets, float FireDelay)
{
	// 모든 클라이언트에게 샷건 발사를 알림
	MulticastShotgunFire(TraceHitTargets);
}

// 서버에서 샷건 발사를 검증하는 메서드 (TraceHitTargets: 샷건 타격 위치들, FireDelay: 발사 딜레이)
bool UCombatComponent::ServerShotgunFire_Validate(const TArray<FVector_NetQuantize>& TraceHitTargets, float FireDelay)
{
	if (EquippedWeapon)
	{
		// 무기의 발사 딜레이와 파라미터의 발사 딜레이가 거의 같은지 검증
		bool bNearlyEqual = FMath::IsNearlyEqual(EquippedWeapon->FireDelay, FireDelay, 0.001f);
		return bNearlyEqual;
	}
	return true;
}

// 모든 클라이언트에게 샷건 발사를 알리는 메서드 (TraceHitTargets: 샷건 타격 위치들)
void UCombatComponent::MulticastShotgunFire_Implementation(const TArray<FVector_NetQuantize>& TraceHitTargets)
{
	// 로컬 컨트롤이고 권한이 없는 클라이언트는 리턴
	if (Character && Character->IsLocallyControlled() && !Character->HasAuthority()) return;
	ShotgunLocalFire(TraceHitTargets);
}

// 로컬에서 발사를 처리하는 메서드 (TraceHitTarget: 타격 위치)
void UCombatComponent::LocalFire(const FVector_NetQuantize& TraceHitTarget)
{
	if (EquippedWeapon == nullptr) return;
	if (Character && CombatState == ECombatState::ECS_Unoccupied)
	{
		// 발사 애니메이션 재생 및 무기 발사
		Character->PlayFireMontage(bAiming);
		EquippedWeapon->Fire(TraceHitTarget);
	}
}
// 샷건의 로컬 발사를 처리하는 메서드 (TraceHitTargets: 샷건 타격 위치들)
void UCombatComponent::ShotgunLocalFire(const TArray<FVector_NetQuantize>& TraceHitTargets)
{
	AShotgun* Shotgun = Cast<AShotgun>(EquippedWeapon);  // 현재 장착한 무기가 샷건인지 확인
	if (Shotgun == nullptr || Character == nullptr) return;
	if (CombatState == ECombatState::ECS_Reloading || CombatState == ECombatState::ECS_Unoccupied)
	{
		bLocallyReloading = false;
		Character->PlayFireMontage(bAiming);
		Shotgun->FireShotgun(TraceHitTargets);  // 샷건 발사 처리
		CombatState = ECombatState::ECS_Unoccupied;
	}
}

// 무기 장착 처리를 하는 메서드 (WeaponToEquip: 장착할 무기)
void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr) return;
	if (CombatState != ECombatState::ECS_Unoccupied) return;

	if (WeaponToEquip->GetWeaponType() == EWeaponType::EWT_Flag)
	{
		Character->Crouch();
		bHoldingTheFlag = true;
		WeaponToEquip->SetWeaponState(EWeaponState::EWS_Equipped);
		AttachFlagToLeftHand(WeaponToEquip);  // 깃발을 왼손에 장착
		WeaponToEquip->SetOwner(Character);
		TheFlag = WeaponToEquip;
	}
	else
	{
		if (EquippedWeapon != nullptr && SecondaryWeapon == nullptr)
		{
			EquipSecondaryWeapon(WeaponToEquip);  // 부착 무기 장착 처리
		}
		else
		{
			EquipPrimaryWeapon(WeaponToEquip);  // 주착 무기 장착 처리
		}

		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
	}
}

// 무기 교체 처리를 하는 메서드
void UCombatComponent::SwapWeapons()
{
	if (CombatState != ECombatState::ECS_Unoccupied || Character == nullptr || !Character->HasAuthority()) return;

	Character->PlaySwapMontage();
	CombatState = ECombatState::ECS_SwappingWeapons;
	Character->bFinishedSwapping = false;
	if (SecondaryWeapon) SecondaryWeapon->EnableCustomDepth(false);
}

// 주 무기 장착 처리를 하는 메서드 (WeaponToEquip: 장착할 무기)
void UCombatComponent::EquipPrimaryWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip == nullptr) return;
	DropEquippedWeapon();  // 현재 장착한 무기 드랍
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	AttachActorToRightHand(EquippedWeapon);  // 무기를 오른손에 장착
	EquippedWeapon->SetOwner(Character);
	EquippedWeapon->SetHUDAmmo();  // HUD에 탄약 표시 업데이트
	UpdateCarriedAmmo();
	PlayEquipWeaponSound(WeaponToEquip);
	ReloadEmptyWeapon();
}

// 부 무기 장착 처리를 하는 메서드 (WeaponToEquip: 장착할 무기)
void UCombatComponent::EquipSecondaryWeapon(AWeapon* WeaponToEquip)
{
	if (WeaponToEquip == nullptr) return;
	SecondaryWeapon = WeaponToEquip;
	SecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);
	AttachActorToBackpack(WeaponToEquip);  // 무기를 백팩에 장착
	PlayEquipWeaponSound(WeaponToEquip);
	SecondaryWeapon->SetOwner(Character);
}
// 'Aiming' 속성 변경을 처리하는 메서드
void UCombatComponent::OnRep_Aiming()
{
	// 캐릭터가 로컬에서 제어되는 경우에만 조준 중인지 업데이트
	if (Character && Character->IsLocallyControlled())
	{
		bAiming = bAimButtonPressed;
	}
}

// 현재 장착된 무기를 드랍하는 메서드
void UCombatComponent::DropEquippedWeapon()
{
	// 현재 장착된 무기가 있으면 드랍
	if (EquippedWeapon)
	{
		EquippedWeapon->Dropped();
	}
}

// 오른손에 배우를 부착하는 메서드 (ActorToAttach: 부착할 배우)
void UCombatComponent::AttachActorToRightHand(AActor* ActorToAttach)
{
	// 캐릭터 또는 캐릭터의 매쉬, 부착할 배우가 없으면 리턴
	if (Character == nullptr || Character->GetMesh() == nullptr || ActorToAttach == nullptr) return;
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (HandSocket)
	{
		// 오른손 소켓에 배우 부착
		HandSocket->AttachActor(ActorToAttach, Character->GetMesh());
	}
}

// 왼손에 깃발을 부착하는 메서드 (Flag: 부착할 깃발)
void UCombatComponent::AttachFlagToLeftHand(AWeapon* Flag)
{
	// 캐릭터 또는 캐릭터의 매쉬, 부착할 깃발이 없으면 리턴
	if (Character == nullptr || Character->GetMesh() == nullptr || Flag == nullptr) return;
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("FlagSocket"));
	if (HandSocket)
	{
		// 깃발 소켓에 깃발 부착
		HandSocket->AttachActor(Flag, Character->GetMesh());
	}
}

// 왼손에 배우를 부착하는 메서드 (ActorToAttach: 부착할 배우)
void UCombatComponent::AttachActorToLeftHand(AActor* ActorToAttach)
{
	// 캐릭터 또는 캐릭터의 매쉬, 부착할 배우, 장착된 무기가 없으면 리턴
	if (Character == nullptr || Character->GetMesh() == nullptr || ActorToAttach == nullptr || EquippedWeapon == nullptr) return;
	bool bUsePistolSocket =
		EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Pistol ||
		EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SubmachineGun;
	FName SocketName = bUsePistolSocket ? FName("PistolSocket") : FName("LeftHandSocket");
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(SocketName);
	if (HandSocket)
	{
		// 선택된 소켓에 배우 부착
		HandSocket->AttachActor(ActorToAttach, Character->GetMesh());
	}
}

// 백팩에 배우를 부착하는 메서드 (ActorToAttach: 부착할 배우)
void UCombatComponent::AttachActorToBackpack(AActor* ActorToAttach)
{
	// 캐릭터 또는 캐릭터의 매쉬, 부착할 배우가 없으면 리턴
	if (Character == nullptr || Character->GetMesh() == nullptr || ActorToAttach == nullptr) return;
	const USkeletalMeshSocket* BackpackSocket = Character->GetMesh()->GetSocketByName(FName("BackpackSocket"));
	if (BackpackSocket)
	{
		// 백팩 소켓에 배우 부착
		BackpackSocket->AttachActor(ActorToAttach, Character->GetMesh());
	}
}

// 소유하고 있는 탄약을 업데이트하는 메서드
void UCombatComponent::UpdateCarriedAmmo()
{
	if (EquippedWeapon == nullptr) return;
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}

	Controller = Controller == nullptr ? Cast<ALetsShootPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		// HUD에 현재 탄약량 표시 업데이트
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
}

// 무기 장착 사운드를 재생하는 메서드 (WeaponToEquip: 장착할 무기)
void UCombatComponent::PlayEquipWeaponSound(AWeapon* WeaponToEquip)
{
	if (Character && WeaponToEquip && WeaponToEquip->EquipSound)
	{
		// 장착 사운드 재생
		UGameplayStatics::PlaySoundAtLocation(
			this,
			WeaponToEquip->EquipSound,
			Character->GetActorLocation()
		);
	}
}
// 빈 무기를 재장전하는 메서드
void UCombatComponent::ReloadEmptyWeapon()
{
	// 현재 장착된 무기가 있고, 그 무기가 빈 경우 재장전
	if (EquippedWeapon && EquippedWeapon->IsEmpty())
	{
		Reload();
	}
}

// 무기를 재장전하는 메서드
void UCombatComponent::Reload()
{
	// 현재 보유한 탄약이 있고, 전투 상태가 '비활성'이며, 장착된 무기가 있고, 그 무기가 가득 차지 않았으며, 로컬에서 재장전 중이 아닌 경우
	if (CarriedAmmo > 0 && CombatState == ECombatState::ECS_Unoccupied && EquippedWeapon && !EquippedWeapon->IsFull() && !bLocallyReloading)
	{
		ServerReload();
		HandleReload();
		bLocallyReloading = true; // 재장전 중으로 상태 변경
	}
}

// 서버에서 재장전을 처리하는 메서드
void UCombatComponent::ServerReload_Implementation()
{
	// 캐릭터 또는 장착된 무기가 없는 경우 리턴
	if (Character == nullptr || EquippedWeapon == nullptr) return;

	CombatState = ECombatState::ECS_Reloading; // 전투 상태를 '재장전 중'으로 변경
	if (!Character->IsLocallyControlled()) HandleReload();
}

// 재장전을 완료하는 메서드
void UCombatComponent::FinishReloading()
{
	if (Character == nullptr) return;
	bLocallyReloading = false; // 재장전 중 상태 해제
	if (Character->HasAuthority())
	{
		CombatState = ECombatState::ECS_Unoccupied; // 전투 상태를 '비활성'으로 변경
		UpdateAmmoValues(); // 탄약 값 업데이트
	}
	if (bFireButtonPressed)
	{
		Fire(); // 발사 버튼이 눌린 경우 발사
	}
}

// 스왑을 완료하는 메서드
void UCombatComponent::FinishSwap()
{
	if (Character && Character->HasAuthority())
	{
		CombatState = ECombatState::ECS_Unoccupied; // 전투 상태를 '비활성'으로 변경
	}
	if (Character) Character->bFinishedSwapping = true; // 캐릭터의 스왑 완료 상태 설정
	if (SecondaryWeapon) SecondaryWeapon->EnableCustomDepth(true);
}

// 스왑 후 무기를 부착하는 메서드
void UCombatComponent::FinishSwapAttachWeapons()
{
	PlayEquipWeaponSound(SecondaryWeapon); // 무기 장착 사운드 재생

	if (Character == nullptr || !Character->HasAuthority()) return;
	AWeapon* TempWeapon = EquippedWeapon;
	EquippedWeapon = SecondaryWeapon;
	SecondaryWeapon = TempWeapon;

	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped); // 장착된 무기 상태 설정
	AttachActorToRightHand(EquippedWeapon); // 오른손에 무기 부착
	EquippedWeapon->SetHUDAmmo(); // HUD에 탄약 설정
	UpdateCarriedAmmo(); // 보유한 탄약 업데이트

	SecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary); // 부착된 두 번째 무기의 상태 설정
	AttachActorToBackpack(SecondaryWeapon); // 백팩에 두 번째 무기 부착
}

// 탄약 값을 업데이트하는 메서드
void UCombatComponent::UpdateAmmoValues()
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;
	int32 ReloadAmount = AmountToReload(); // 재장전할 탄약 수량
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[EquippedWeapon->GetWeaponType()] -= ReloadAmount;
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}
	Controller = Controller == nullptr ? Cast<ALetsShootPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo); // HUD에 현재 탄약량 표시 업데이트
	}
	EquippedWeapon->AddAmmo(ReloadAmount); // 장착된 무기에 탄약 추가
}
// 샷건의 탄약 값을 업데이트하는 메서드
void UCombatComponent::UpdateShotgunAmmoValues()
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;

	// 장착된 무기 타입에 해당하는 탄약이 있다면 그 탄약을 1만큼 감소
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[EquippedWeapon->GetWeaponType()] -= 1;
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}

	// 컨트롤러가 없다면 캐릭터의 컨트롤러를 얻어옴
	Controller = Controller == nullptr ? Cast<ALetsShootPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);  // HUD에 탄약 갯수 업데이트
	}

	EquippedWeapon->AddAmmo(1);  // 장착된 무기에 탄약 추가
	bCanFire = true;  // 발사 가능 상태로 설정

	if (EquippedWeapon->IsFull() || CarriedAmmo == 0)
	{
		JumpToShotgunEnd();  // 샷건이 가득 차거나 탄약이 없다면 재장전 종료
	}
}

// 수류탄 정보가 변경됐을 때 HUD를 업데이트하는 메서드
void UCombatComponent::OnRep_Grenades()
{
	UpdateHUDGrenades();
}

// 샷건 재장전의 마지막 부분으로 이동하는 메서드
void UCombatComponent::JumpToShotgunEnd()
{
	// Jump to ShotgunEnd section
	UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance();
	if (AnimInstance && Character->GetReloadMontage())
	{
		AnimInstance->Montage_JumpToSection(FName("ShotgunEnd"));  // 애니메이션의 샷건 재장전 종료 섹션으로 이동
	}
}

// 수류탄 투척이 완료된 후 처리하는 메서드
void UCombatComponent::ThrowGrenadeFinished()
{
	CombatState = ECombatState::ECS_Unoccupied;  // 전투 상태를 '비활성'으로 설정
	AttachActorToRightHand(EquippedWeapon);  // 오른손에 무기를 부착
}

// 수류탄을 투척하는 메서드
void UCombatComponent::LaunchGrenade()
{
	ShowAttachedGrenade(false);  // 부착된 수류탄 숨김
	if (Character && Character->IsLocallyControlled())
	{
		ServerLaunchGrenade(HitTarget);  // 서버에서 수류탄 투척 처리
	}
}

// 서버에서 수류탄을 투척하는 메서드, 파라미터로 투척 대상 위치를 받음
void UCombatComponent::ServerLaunchGrenade_Implementation(const FVector_NetQuantize& Target)
{
	if (Character && GrenadeClass && Character->GetAttachedGrenade())
	{
		const FVector StartingLocation = Character->GetAttachedGrenade()->GetComponentLocation();  // 투척 시작 위치를 수류탄 위치로 설정
		FVector ToTarget = Target - StartingLocation;  // 목표 지점을 향하는 벡터 계산
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = Character;
		SpawnParams.Instigator = Character;
		UWorld* World = GetWorld();
		if (World)
		{
			World->SpawnActor<AProjectile>(
				GrenadeClass,
				StartingLocation,
				ToTarget.Rotation(),
				SpawnParams
				);  // 세계에 수류탄 액터 생성
		}
	}
}

// 전투 상태 변경에 대한 처리를 하는 메서드
void UCombatComponent::OnRep_CombatState()
{
	switch (CombatState)
	{
	case ECombatState::ECS_Reloading:
		if (Character && !Character->IsLocallyControlled()) HandleReload();  // 재장전 처리
		break;
	case ECombatState::ECS_Unoccupied:
		if (bFireButtonPressed)
		{
			Fire();  // 발사 처리
		}
		break;
	case ECombatState::ECS_ThrowingGrenade:
		if (Character && !Character->IsLocallyControlled())
		{
			Character->PlayThrowGrenadeMontage();  // 수류탄 투척 애니메이션 재생
			AttachActorToLeftHand(EquippedWeapon);  // 왼손에 무기 부착
			ShowAttachedGrenade(true);  // 수류탄 보이게 설정
		}
		break;
	case ECombatState::ECS_SwappingWeapons:
		if (Character && !Character->IsLocallyControlled())
		{
			Character->PlaySwapMontage();  // 무기 교체 애니메이션 재생
		}
		break;
	}
}
// 재장전을 처리하는 메서드
void UCombatComponent::HandleReload()
{
	if (Character)
	{
		Character->PlayReloadMontage();  // 재장전 애니메이션 재생
	}
}

// 재장전해야 할 탄약의 양을 반환하는 메서드
int32 UCombatComponent::AmountToReload()
{
	if (EquippedWeapon == nullptr) return 0;  // 장착된 무기가 없다면 0 반환
	int32 RoomInMag = EquippedWeapon->GetMagCapacity() - EquippedWeapon->GetAmmo();  // 탄창의 여유 공간 계산

	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))  // 가지고 있는 탄약 타입이 장착된 무기의 타입과 일치하는 경우
	{
		int32 AmountCarried = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
		int32 Least = FMath::Min(RoomInMag, AmountCarried);  // 탄창의 여유 공간과 가지고 있는 탄약 중 최소값 계산
		return FMath::Clamp(RoomInMag, 0, Least);  // 재장전해야 할 탄약의 양 반환
	}
	return 0;
}

// 수류탄을 던지는 메서드
void UCombatComponent::ThrowGrenade()
{
	if (Grenades == 0) return;  // 수류탄이 없다면 반환
	if (CombatState != ECombatState::ECS_Unoccupied || EquippedWeapon == nullptr) return;  // 전투 상태가 비활성 상태가 아니거나, 장착된 무기가 없다면 반환
	CombatState = ECombatState::ECS_ThrowingGrenade;  // 전투 상태를 수류탄 던지는 상태로 변경
	if (Character)
	{
		Character->PlayThrowGrenadeMontage();  // 수류탄 던지는 애니메이션 재생
		AttachActorToLeftHand(EquippedWeapon);  // 왼손에 무기 부착
		ShowAttachedGrenade(true);  // 수류탄 보이게 설정
	}
	if (Character && !Character->HasAuthority())
	{
		ServerThrowGrenade();  // 서버에서 수류탄 던지는 메서드 호출
	}
	if (Character && Character->HasAuthority())
	{
		Grenades = FMath::Clamp(Grenades - 1, 0, MaxGrenades);  // 수류탄 갯수 감소
		UpdateHUDGrenades();  // HUD의 수류탄 갯수 업데이트
	}
}

// 서버에서 수류탄을 던지는 메서드
void UCombatComponent::ServerThrowGrenade_Implementation()
{
	if (Grenades == 0) return;  // 수류탄이 없다면 반환
	CombatState = ECombatState::ECS_ThrowingGrenade;  // 전투 상태를 수류탄 던지는 상태로 변경
	if (Character)
	{
		Character->PlayThrowGrenadeMontage();  // 수류탄 던지는 애니메이션 재생
		AttachActorToLeftHand(EquippedWeapon);  // 왼손에 무기 부착
		ShowAttachedGrenade(true);  // 수류탄 보이게 설정
	}
	Grenades = FMath::Clamp(Grenades - 1, 0, MaxGrenades);  // 수류탄 갯수 감소
	UpdateHUDGrenades();  // HUD의 수류탄 갯수 업데이트
}

// HUD의 수류탄 갯수를 업데이트하는 메서드
void UCombatComponent::UpdateHUDGrenades()
{
	Controller = Controller == nullptr ? Cast<ALetsShootPlayerController>(Character->Controller) : Controller;  // 컨트롤러 설정
	if (Controller)
	{
		Controller->SetHUDGrenades(Grenades);  // HUD의 수류탄 갯수 설정
	}
}

// 무기를 교체해야 하는지 여부를 판단하는 메서드
bool UCombatComponent::ShouldSwapWeapons()
{
	return (EquippedWeapon != nullptr && SecondaryWeapon != nullptr);  // 장착된 무기와 부착된 무기 둘 다 있는 경우에만 true 반환
}

// 수류탄 보이게 설정하는 메서드
// bShowGrenade: 수류탄을 보이게 할지 여부
void UCombatComponent::ShowAttachedGrenade(bool bShowGrenade)
{
	if (Character && Character->GetAttachedGrenade())
	{
		Character->GetAttachedGrenade()->SetVisibility(bShowGrenade);  // 수류탄 보이게 설정
	}
}

// 장착된 무기 변경 시 처리하는 메서드
void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon && Character)
	{
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);  // 무기 상태를 장착된 상태로 설정
		AttachActorToRightHand(EquippedWeapon);  // 오른손에 무기 부착
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;  // 캐릭터 회전 방향 설정
		Character->bUseControllerRotationYaw = true;  // 컨트롤러의 회전 방향 설정
		PlayEquipWeaponSound(EquippedWeapon);  // 무기 장착 사운드 재생
		EquippedWeapon->EnableCustomDepth(false);  // 커스텀 뎁스 비활성화
		EquippedWeapon->SetHUDAmmo();  // HUD의 탄약 설정
	}
}


// 보조 무기가 장착되었을 때 처리하는 메서드
void UCombatComponent::OnRep_SecondaryWeapon()
{
	if (SecondaryWeapon && Character)
	{
		SecondaryWeapon->SetWeaponState(EWeaponState::EWS_EquippedSecondary);  // 보조 무기의 상태를 '보조 무기로 장착됨'으로 설정
		AttachActorToBackpack(SecondaryWeapon);  // 보조 무기를 백팩에 부착
		PlayEquipWeaponSound(EquippedWeapon);  // 무기 장착 사운드 재생
	}
}

// 조준선 아래에 있는 객체를 추적하는 메서드
// TraceHitResult: 객체 추적 결과
void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);  // 뷰포트 크기 가져오기
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);  // 조준선 위치 설정
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(  // 스크린에서 월드 좌표로 변환
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);

	if (bScreenToWorld)  // 스크린에서 월드 좌표로 변환에 성공했다면
	{
		FVector Start = CrosshairWorldPosition;  // 시작점 설정

		if (Character)  // 캐릭터가 있다면
		{
			float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();  // 캐릭터와의 거리 계산
			Start += CrosshairWorldDirection * (DistanceToCharacter + 100.f);  // 시작점 갱신
		}

		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;  // 끝점 설정

		GetWorld()->LineTraceSingleByChannel(  // 선 추적 실행
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);
		if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractWithCrosshairsInterface>())  // 추적 결과에서 객체를 얻었고, 그 객체가 UInteractWithCrosshairsInterface를 구현한다면
		{
			HUDPackage.CrosshairsColor = FLinearColor::Red;  // 조준선 색상을 빨간색으로 설정
		}
		else
		{
			HUDPackage.CrosshairsColor = FLinearColor::White;  // 조준선 색상을 흰색으로 설정
		}
	}
}

// HUD의 조준선을 설정하는 메서드
// DeltaTime: 이전 프레임에서 현재 프레임까지의 시간
void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	// 캐릭터나 컨트롤러가 없다면 함수 종료
	if (Character == nullptr || Character->Controller == nullptr) return;

	// 컨트롤러 설정
	Controller = Controller == nullptr ? Cast<ALetsShootPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		// HUD 설정
		HUD = HUD == nullptr ? Cast<ALetsShootHUD>(Controller->GetHUD()) : HUD;
		if (HUD)
		{
			// 장착한 무기가 있다면
			if (EquippedWeapon)
			{
				// 조준선 위치를 무기의 위치에 따라 설정
				HUDPackage.CrosshairsCenter = EquippedWeapon->CrosshairsCenter;
				HUDPackage.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
				HUDPackage.CrosshairsRight = EquippedWeapon->CrosshairsRight;
				HUDPackage.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;
				HUDPackage.CrosshairsTop = EquippedWeapon->CrosshairsTop;
			}
			else
			{
				// 무기가 없다면 조준선 위치를 null로 설정
				HUDPackage.CrosshairsCenter = nullptr;
				HUDPackage.CrosshairsLeft = nullptr;
				HUDPackage.CrosshairsRight = nullptr;
				HUDPackage.CrosshairsBottom = nullptr;
				HUDPackage.CrosshairsTop = nullptr;
			}

			// 조준선 퍼짐 계산
			FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelocityMultiplierRange(0.f, 1.f);
			FVector Velocity = Character->GetVelocity();
			Velocity.Z = 0.f;

			// 속도에 따른 조준선 요소 계산
			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

			// 캐릭터가 떨어지고 있다면
			if (Character->GetCharacterMovement()->IsFalling())
			{
				// 공중에 있는 경우의 조준선 요소를 계산
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
			}
			else
			{
				// 공중에 없는 경우의 조준선 요소를 계산
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
			}

			// 조준 중이라면
			if (bAiming)
			{
				// 조준 중일 때의 조준선 요소를 계산
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.58f, DeltaTime, 30.f);
			}
			else
			{
				// 조준 중이 아닐 때의 조준선 요소를 계산
				CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
			}

			// 총을 쏠 때의 조준선 요소를 계산
			CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 40.f);

			// 모든 요소를 합하여 조준선 퍼짐 계산
			HUDPackage.CrosshairSpread =
				0.5f +
				CrosshairVelocityFactor +
				CrosshairInAirFactor -
				CrosshairAimFactor +
				CrosshairShootingFactor;

			// HUD에 패키지 설정
			HUD->SetHUDPackage(HUDPackage);
		}
	}
}

// DeltaTime : 이전 프레임에서 현재 프레임까지의 시간
void UCombatComponent::InterpFOV(float DeltaTime)
{
	// 무기가 없다면 함수 종료
	if (EquippedWeapon == nullptr) return;

	// 조준 중이라면
	if (bAiming)
	{
		// 조준 중인 경우의 시야 범위 계산
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeapon->GetZoomedFOV(), DeltaTime, EquippedWeapon->GetZoomInterpSpeed());
	}
	else
	{
		// 조준 중이 아닌 경우의 시야 범위 계산
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, ZoomInterpSpeed);
	}

	// 캐릭터와 그의 카메라가 있다면
	if (Character && Character->GetFollowCamera())
	{
		// 카메라의 시야 범위 설정
		Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}
}


// bIsAiming : 조준 중인지를 나타내는 변수
void UCombatComponent::SetAiming(bool bIsAiming)
{
	// 캐릭터나 장착한 무기가 없다면 함수 종료
	if (Character == nullptr || EquippedWeapon == nullptr) return;

	bAiming = bIsAiming;
	ServerSetAiming(bIsAiming);
	if (Character)
	{
		// 캐릭터의 최대 이동 속도를 조준 중인지 여부에 따라 설정
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
	if (Character->IsLocallyControlled() && EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle)
	{
		// 조준 중이고 장착한 무기가 스나이퍼 라이플일 경우 스나이퍼 조준점 위젯 표시
		Character->ShowSniperScopeWidget(bIsAiming);
	}
	if (Character->IsLocallyControlled()) bAimButtonPressed = bIsAiming;
}

// bIsAiming : 조준 중인지를 나타내는 변수
void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
	if (Character)
	{
		// 캐릭터의 최대 이동 속도를 조준 중인지 여부에 따라 설정
		Character->GetCharacterMovement()->MaxWalkSpeed = bIsAiming ? AimWalkSpeed : BaseWalkSpeed;
	}
}

bool UCombatComponent::CanFire()
{
	// 무기가 없거나 무기가 비어 있거나 총을 쏠 수 없거나 재장전 중일 경우 불가능
	if (EquippedWeapon == nullptr) return false;
	if (!EquippedWeapon->IsEmpty() && bCanFire && CombatState == ECombatState::ECS_Reloading && EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Shotgun) return true;
	if (bLocallyReloading) return false;
	// 그 외의 경우에는 무기가 비어 있지 않고 총을 쏠 수 있으며 전투 상태가 비어 있을 경우 가능
	return !EquippedWeapon->IsEmpty() && bCanFire && CombatState == ECombatState::ECS_Unoccupied;
}

void UCombatComponent::OnRep_CarriedAmmo()
{
	// 컨트롤러를 설정하고, HUD에 탄약 수를 설정
	Controller = Controller == nullptr ? Cast<ALetsShootPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDCarriedAmmo(CarriedAmmo);
	}
	bool bJumpToShotgunEnd =
		CombatState == ECombatState::ECS_Reloading &&
		EquippedWeapon != nullptr &&
		EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Shotgun &&
		CarriedAmmo == 0;
	if (bJumpToShotgunEnd)
	{
		JumpToShotgunEnd();
	}
}

void UCombatComponent::InitializeCarriedAmmo()
{
	// 각 무기 유형별로 초기 탄약 수를 설정
	CarriedAmmoMap.Emplace(EWeaponType::EWT_AssaultRifle, StartingARAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_RocketLauncher, StartingRocketAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Pistol, StartingPistolAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_SubmachineGun, StartingSMGAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_Shotgun, StartingShotgunAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_SniperRifle, StartingShotgunAmmo);
	CarriedAmmoMap.Emplace(EWeaponType::EWT_GrenadeLauncher, StartingGrenadeLauncherAmmo);
}

void UCombatComponent::OnRep_HoldingTheFlag()
{
	// 플래그를 들고 있고, 캐릭터가 로컬로 제어되는 경우 캐릭터를 웅크리게 함
	if (bHoldingTheFlag && Character && Character->IsLocallyControlled())
	{
		Character->Crouch();
	}
}

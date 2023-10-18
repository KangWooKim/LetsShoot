// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "LetsShoot/Character/LetsShootCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimationAsset.h"
#include "Components/SkeletalMeshComponent.h"
#include "Casing.h"
#include "Engine/SkeletalMeshSocket.h"
#include "LetsShoot/PlayerController/LetsShootPlayerController.h"
#include "LetsShoot/LetsShootComponents/CombatComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
// AWeapon 클래스 생성자
AWeapon::AWeapon()
{
	// 액터의 틱 가능 여부 설정. 이 액터는 틱되지 않습니다.
	PrimaryActorTick.bCanEverTick = false;

	// 복제 가능 여부 설정. 이 액터는 복제 가능합니다.
	bReplicates = true;

	// 움직임 복제 여부 설정. 이 액터의 움직임은 복제됩니다.
	SetReplicateMovement(true);

	// 무기 메시 컴포넌트 생성 및 루트 컴포넌트로 설정
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	// 무기 메시의 충돌 응답 설정
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECCollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 커스텀 뎁스 활성화 및 설정
	EnableCustomDepth(true);
	WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
	WeaponMesh->MarkRenderStateDirty();

	// 영역 구 컴포넌트 생성 및 설정
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 픽업 위젯 컴포넌트 생성 및 설정
	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(RootComponent);
}

// 커스텀 뎁스 활성화 메서드
// bEnable: 커스텀 뎁스를 활성화할지 여부를 결정하는 불리언 값입니다. 이 값이 true이면 커스텀 뎁스가 활성화되고, false이면 비활성화됩니다.
void AWeapon::EnableCustomDepth(bool bEnable)
{
	if (WeaponMesh)
	{
		// 무기 메시의 커스텀 뎁스 설정
		WeaponMesh->SetRenderCustomDepth(bEnable);
	}
}

// 게임이 시작되었을 때의 동작을 정의하는 메서드
void AWeapon::BeginPlay()
{
	// 부모 클래스의 BeginPlay 호출
	Super::BeginPlay();

	// 영역 구의 충돌 설정
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECCollisionResponse::ECR_Overlap);
	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AWeapon::OnSphereEndOverlap);

	if (PickupWidget)
	{
		// 픽업 위젯 비활성화
		PickupWidget->SetVisibility(false);
	}
}


// 무기에 대한 틱 메서드
void AWeapon::Tick(float DeltaTime)
{
	// 부모 클래스의 틱 메서드 호출
	Super::Tick(DeltaTime);
}

// 복제되는 속성들을 설정하는 메서드
void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	// 부모 클래스의 복제 속성 설정 메서드 호출
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 무기 상태와 서버 측 되감기 사용 여부를 복제되는 속성으로 설정
	DOREPLIFETIME(AWeapon, WeaponState);
	DOREPLIFETIME_CONDITION(AWeapon, bUseServerSideRewind, COND_OwnerOnly);
}

// 구 컴포넌트와의 오버랩이 시작될 때 호출되는 메서드
void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ALetsShootCharacter* LetsShootCharacter = Cast<ALetsShootCharacter>(OtherActor);

	// 블래스터 캐릭터가 오버랩이 시작된 경우
	if (LetsShootCharacter)
	{
		// 현재 무기가 플래그 형태이고, 오버랩된 캐릭터가 동일 팀일 경우 리턴
		if (WeaponType == EWeaponType::EWT_Flag && LetsShootCharacter->GetTeam() == Team) return;

		// 오버랩된 캐릭터가 이미 플래그를 소유하고 있는 경우 리턴
		if (LetsShootCharacter->IsHoldingTheFlag()) return;

		// 오버랩된 캐릭터에게 현재 오버랩된 무기를 설정
		LetsShootCharacter->SetOverlappingWeapon(this);
	}
}

// 구 컴포넌트와의 오버랩이 종료될 때 호출되는 메서드
void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ALetsShootCharacter* LetsShootCharacter = Cast<ALetsShootCharacter>(OtherActor);
	if (LetsShootCharacter)
	{
		// 현재 무기가 플래그 형태이고, 오버랩된 캐릭터가 동일 팀일 경우 리턴
		if (WeaponType == EWeaponType::EWT_Flag && LetsShootCharacter->GetTeam() == Team) return;

		// 오버랩된 캐릭터가 이미 플래그를 소유하고 있는 경우 리턴
		if (LetsShootCharacter->IsHoldingTheFlag()) return;

		// 오버랩된 캐릭터에게 오버랩된 무기를 null로 설정
		LetsShootCharacter->SetOverlappingWeapon(nullptr);
	}
}

// HUD에 표시되는 탄약 정보를 설정하는 메서드
void AWeapon::SetHUDAmmo()
{
	LetsShootOwnerCharacter = LetsShootOwnerCharacter == nullptr ? Cast<ALetsShootCharacter>(GetOwner()) : LetsShootOwnerCharacter;
	if (LetsShootOwnerCharacter)
	{
		LetsShootOwnerController = LetsShootOwnerController == nullptr ? Cast<ALetsShootPlayerController>(LetsShootOwnerCharacter->Controller) : LetsShootOwnerController;
		if (LetsShootOwnerController)
		{
			// 컨트롤러를 통해 HUD의 무기 탄약 정보를 설정
			LetsShootOwnerController->SetHUDWeaponAmmo(Ammo);
		}
	}
}

// 무기를 발사하여 탄약을 소모하는 메서드
void AWeapon::SpendRound()
{
	// 남은 탄약을 하나 감소시키고, 0과 탄창 용량 사이로 클램프
	Ammo = FMath::Clamp(Ammo - 1, 0, MagCapacity);

	// HUD에 탄약 정보 업데이트
	SetHUDAmmo();

	// 서버 권한이 있는 경우 클라이언트에 탄약 정보를 업데이트
	if (HasAuthority())
	{
		ClientUpdateAmmo(Ammo);
	}
	else
	{
		++Sequence;
	}
}


// 클라이언트 측에서 서버로부터 받은 탄약 정보를 업데이트하는 메서드
void AWeapon::ClientUpdateAmmo_Implementation(int32 ServerAmmo)
{
	// 서버 권한이 있는 경우 리턴
	if (HasAuthority()) return;

	// 서버로부터 받은 탄약 정보를 저장
	Ammo = ServerAmmo;

	// 시퀀스 감소
	--Sequence;

	// 탄약을 시퀀스만큼 감소시키고, HUD에 탄약 정보를 업데이트
	Ammo -= Sequence;
	SetHUDAmmo();
}

// 탄약을 추가하는 메서드
void AWeapon::AddAmmo(int32 AmmoToAdd)
{
	// 탄약을 추가하고, 0과 탄창 용량 사이로 클램프
	Ammo = FMath::Clamp(Ammo + AmmoToAdd, 0, MagCapacity);

	// HUD에 탄약 정보를 업데이트하고, 클라이언트에 탄약 추가 정보를 전달
	SetHUDAmmo();
	ClientAddAmmo(AmmoToAdd);
}

// 클라이언트 측에서 탄약을 추가하는 메서드
void AWeapon::ClientAddAmmo_Implementation(int32 AmmoToAdd)
{
	// 서버 권한이 있는 경우 리턴
	if (HasAuthority()) return;

	// 탄약을 추가하고, 0과 탄창 용량 사이로 클램프
	Ammo = FMath::Clamp(Ammo + AmmoToAdd, 0, MagCapacity);

	// 블래스터 소유 캐릭터를 가져오고, 해당 캐릭터의 전투 상태를 확인한 후, 탄창이 가득 찬 경우, 샷건 엔드로 이동
	LetsShootOwnerCharacter = LetsShootOwnerCharacter == nullptr ? Cast<ALetsShootCharacter>(GetOwner()) : LetsShootOwnerCharacter;
	if (LetsShootOwnerCharacter && LetsShootOwnerCharacter->GetCombat() && IsFull())
	{
		LetsShootOwnerCharacter->GetCombat()->JumpToShotgunEnd();
	}

	// HUD에 탄약 정보를 업데이트
	SetHUDAmmo();
}

// 무기 소유자에 대한 복제가 이루어졌을 때 호출되는 메서드
void AWeapon::OnRep_Owner()
{
	// 부모 클래스의 무기 소유자 복제 메서드 호출
	Super::OnRep_Owner();

	// 소유자가 없는 경우, 블래스터 소유 캐릭터와 컨트롤러를 null로 설정
	if (Owner == nullptr)
	{
		LetsShootOwnerCharacter = nullptr;
		LetsShootOwnerController = nullptr;
	}
	else
	{
		// 소유자가 존재하는 경우, 블래스터 소유 캐릭터를 설정하고, 해당 캐릭터가 무기를 장착하고 있는지 확인한 후, HUD에 탄약 정보를 업데이트
		LetsShootOwnerCharacter = LetsShootOwnerCharacter == nullptr ? Cast<ALetsShootCharacter>(Owner) : LetsShootOwnerCharacter;
		if (LetsShootOwnerCharacter && LetsShootOwnerCharacter->GetEquippedWeapon() && LetsShootOwnerCharacter->GetEquippedWeapon() == this)
		{
			SetHUDAmmo();
		}
	}
}

// 무기 상태를 설정하는 메서드
void AWeapon::SetWeaponState(EWeaponState State)
{
	// 무기 상태를 설정하고, 무기 상태 설정 이벤트를 호출
	WeaponState = State;
	OnWeaponStateSet();
}


// 무기의 상태에 따라 적절한 처리를 수행하는 메서드
void AWeapon::OnWeaponStateSet()
{
	switch (WeaponState)
	{
		// 무기가 장착된 상태일 때의 처리
	case EWeaponState::EWS_Equipped:
		OnEquipped();
		break;
		// 무기가 보조 장비로 장착된 상태일 때의 처리
	case EWeaponType::EWT_EquippedSecondary:
		OnEquippedSecondary();
		break;
		// 무기가 드랍된 상태일 때의 처리
	case EWeaponState::EWS_Dropped:
		OnDropped();
		break;
	}
}

// 핑이 너무 높은 경우, 서버 측 리와인드를 사용하지 않도록 설정하는 메서드
void AWeapon::OnPingTooHigh(bool bPingTooHigh) // bPingTooHigh: 핑이 너무 높은지를 나타내는 변수
{
	bUseServerSideRewind = !bPingTooHigh;
}

// 무기 상태에 대한 복제가 이루어졌을 때 호출되는 메서드
void AWeapon::OnRep_WeaponState()
{
	OnWeaponStateSet();
}

// 무기가 장착되었을 때 호출되는 메서드
void AWeapon::OnEquipped()
{
	// 픽업 위젯 숨김
	ShowPickupWidget(false);

	// 충돌을 비활성화하고 물리 시뮬레이션과 중력을 비활성화
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetEnableGravity(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 무기가 서브머신건인 경우, 충돌과 중력을 활성화하고 모든 채널에 대한 충돌 응답을 무시
	if (WeaponType == EWeaponType::EWT_SubmachineGun)
	{
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	}

	// 사용자 정의 깊이를 비활성화
	EnableCustomDepth(false);

	// 블래스터 소유 캐릭터를 가져오고, 서버 측 리와인드를 사용하는 경우,
	// 블래스터 소유 컨트롤러를 가져와서 핑이 너무 높은 경우에 대한 델리게이트를 추가
	LetsShootOwnerCharacter = LetsShootOwnerCharacter == nullptr ? Cast<ALetsShootCharacter>(GetOwner()) : LetsShootOwnerCharacter;
	if (LetsShootOwnerCharacter && bUseServerSideRewind)
	{
		LetsShootOwnerController = LetsShootOwnerController == nullptr ? Cast<ALetsShootPlayerController>(LetsShootOwnerCharacter->Controller) : LetsShootOwnerController;
		if (LetsShootOwnerController && HasAuthority() && !LetsShootOwnerController->HighPingDelegate.IsBound())
		{
			LetsShootOwnerController->HighPingDelegate.AddDynamic(this, &AWeapon::OnPingTooHigh);
		}
	}
}


// 무기가 드랍되었을 때 호출되는 메서드
void AWeapon::OnDropped()
{
	// 만약 이 객체가 권한을 가지고 있다면, AreaSphere의 충돌을 쿼리만 가능하도록 설정
	if (HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	// WeaponMesh의 물리 시뮬레이션과 중력을 활성화, 충돌을 쿼리와 물리로 설정
	// 모든 채널의 충돌 응답을 차단하고, Pawn과 Camera 채널의 충돌 응답은 무시하도록 설정
	WeaponMesh->SetSimulatePhysics(true);
	WeaponMesh->SetEnableGravity(true);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECCollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECCollisionResponse::ECR_Ignore);

	// 사용자 정의 깊이 스텐실 값을 파란색으로 설정하고 렌더 상태를 더러움으로 표시
	// 사용자 정의 깊이를 활성화
	WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
	WeaponMesh->MarkRenderStateDirty();
	EnableCustomDepth(true);

	// 블래스터 소유 캐릭터를 가져오고,
	// 블래스터 소유 컨트롤러를 가져와서 핑이 너무 높은 경우에 대한 델리게이트를 제거
	LetsShootOwnerCharacter = LetsShootOwnerCharacter == nullptr ? Cast<ALetsShootCharacter>(GetOwner()) : LetsShootOwnerCharacter;
	if (LetsShootOwnerCharacter)
	{
		LetsShootOwnerController = LetsShootOwnerController == nullptr ? Cast<ALetsShootPlayerController>(LetsShootOwnerCharacter->Controller) : LetsShootOwnerController;
		if (LetsShootOwnerController && HasAuthority() && LetsShootOwnerController->HighPingDelegate.IsBound())
		{
			LetsShootOwnerController->HighPingDelegate.RemoveDynamic(this, &AWeapon::OnPingTooHigh);
		}
	}
}

// 무기가 보조 장비로 장착되었을 때 호출되는 메서드
void AWeapon::OnEquippedSecondary()
{
	// 픽업 위젯 숨김
	// 충돌을 비활성화하고 물리 시뮬레이션과 중력을 비활성화
	// 무기가 서브머신건인 경우, 충돌과 중력을 활성화하고 모든 채널에 대한 충돌 응답을 무시
	ShowPickupWidget(false);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetEnableGravity(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (WeaponType == EWeaponType::EWT_SubmachineGun)
	{
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	}

	// 무기 메시가 있다면, 사용자 정의 깊이 스텐실 값을 탄색으로 설정하고 렌더 상태를 더러움으로 표시
	if (WeaponMesh)
	{
		WeaponMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_TAN);
		WeaponMesh->MarkRenderStateDirty();
	}

	// 블래스터 소유 캐릭터를 가져오고,
	// 블래스터 소유 컨트롤러를 가져와서 핑이 너무 높은 경우에 대한 델리게이트를 제거
	LetsShootOwnerCharacter = LetsShootOwnerCharacter == nullptr ? Cast<ALetsShootCharacter>(GetOwner()) : LetsShootOwnerCharacter;
	if (LetsShootOwnerCharacter)
	{
		LetsShootOwnerController = LetsShootOwnerController == nullptr ? Cast<ALetsShootPlayerController>(LetsShootOwnerCharacter->Controller) : LetsShootOwnerController;
		if (LetsShootOwnerController && HasAuthority() && LetsShootOwnerController->HighPingDelegate.IsBound())
		{
			LetsShootOwnerController->HighPingDelegate.RemoveDynamic(this, &AWeapon::OnPingTooHigh);
		}
	}
}


// 픽업 위젯의 가시성을 설정하는 메서드
void AWeapon::ShowPickupWidget(bool bShowWidget) // bShowWidget: 위젯을 표시할지 여부를 결정하는 불린값
{
	// PickupWidget이 있다면 가시성을 설정
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(bShowWidget);
	}
}

// 무기를 발사하는 메서드
void AWeapon::Fire(const FVector& HitTarget) // HitTarget: 타격 대상의 위치
{
	// 발사 애니메이션이 있다면 애니메이션 재생
	if (FireAnimation)
	{
		WeaponMesh->PlayAnimation(FireAnimation, false);
	}
	// 탄피 클래스가 있다면, 탄피를 방출하는 소켓에서 탄피를 생성
	if (CasingClass)
	{
		const USkeletalMeshSocket* AmmoEjectSocket = WeaponMesh->GetSocketByName(FName("AmmoEject"));
		if (AmmoEjectSocket)
		{
			FTransform SocketTransform = AmmoEjectSocket->GetSocketTransform(WeaponMesh);

			UWorld* World = GetWorld();
			if (World)
			{
				World->SpawnActor<ACasing>(
					CasingClass,
					SocketTransform.GetLocation(),
					SocketTransform.GetRotation().Rotator()
					);
			}
		}
	}
	// 총알을 사용
	SpendRound();
}

// 무기가 드랍되었을 때 호출되는 메서드
void AWeapon::Dropped()
{
	// 무기 상태를 '드랍됨'으로 설정
	SetWeaponState(EWeaponState::EWS_Dropped);

	// 무기 메시를 월드 변환 규칙을 따라서 컴포넌트로부터 분리
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	WeaponMesh->DetachFromComponent(DetachRules);

	// 무기의 소유자와 캐릭터, 컨트롤러를 nullptr로 설정
	SetOwner(nullptr);
	LetsShootOwnerCharacter = nullptr;
	LetsShootOwnerController = nullptr;
}

// 무기의 탄창이 비어있는지 확인하는 메서드
bool AWeapon::IsEmpty()
{
	return Ammo <= 0; // 탄창에 남아있는 총알의 개수가 0이하면 true 반환
}

// 무기의 탄창이 가득 차 있는지 확인하는 메서드
bool AWeapon::IsFull()
{
	return Ammo == MagCapacity; // 탄창에 남아있는 총알의 개수가 탄창 용량과 같으면 true 반환
}

// 스캐터(퍼지는 형태)를 고려한 트레이스 엔드 포인트를 계산하는 메서드
FVector AWeapon::TraceEndWithScatter(const FVector& HitTarget) // HitTarget: 타격 대상의 위치
{
	// 머즐플래시 소켓 참조 획득
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	// 소켓이 없다면 빈 벡터 반환
	if (MuzzleFlashSocket == nullptr) return FVector();

	// 소켓의 변환(위치, 방향, 크기) 정보 획득
	const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());

	// 트레이스 시작 위치를 소켓 위치로 설정
	const FVector TraceStart = SocketTransform.GetLocation();

	// 타격 대상을 향하는 방향 벡터를 정규화
	const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();

	// 스피어(구체) 중심 위치 계산
	const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;

	// 무작위 벡터를 생성하여 스피어 반경 내에서 위치 계산
	const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);

	// 최종 위치 계산
	const FVector EndLoc = SphereCenter + RandVec;
	const FVector ToEndLoc = EndLoc - TraceStart;

	/*
	// 디버그 목적으로 스피어와 라인을 그림
	DrawDebugSphere(GetWorld(), SphereCenter, SphereRadius, 12, FColor::Red, true);
	DrawDebugSphere(GetWorld(), EndLoc, 4.f, 12, FColor::Orange, true);
	DrawDebugLine(
		GetWorld(),
		TraceStart,
		FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size()),
		FColor::Cyan,
		true);*/

		// 최종 트레이스 엔드 포인트를 반환
	return FVector(TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size());
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "LetsShootCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "LetsShoot/Weapon/Weapon.h"
#include "LetsShoot/LetsShootComponents/CombatComponent.h"
#include "LetsShoot/LetsShootComponents/BuffComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "LetsShootAnimInstance.h"
#include "LetsShoot/LetsShoot.h"
#include "LetsShoot/PlayerController/LetsShootPlayerController.h"
#include "LetsShoot/GameMode/LetsShootGameMode.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "LetsShoot/PlayerState/LetsShootPlayerState.h"
#include "LetsShoot/Weapon/WeaponTypes.h"
#include "Components/BoxComponent.h"
#include "LetsShoot/LetsShootComponents/DelayProcessingComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "LetsShoot/GameState/LetsShootGameState.h"
#include "LetsShoot/PlayerStart/TeamFightPlayerStart.h"

ALetsShootCharacter::ALetsShootCharacter()
{
	// 틱이 작동하도록 설정
	PrimaryActorTick.bCanEverTick = true;

	// 카메라 붐을 생성하고 설정
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	// 추적 카메라를 생성하고 설정
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// 컨트롤러의 회전 설정을 사용하지 않도록 설정
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// 오버헤드 위젯 생성
	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	// 컴뱃 컴포넌트를 생성하고 네트워크 복제 설정
	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat"));
	Combat->SetIsReplicated(true);

	// 버프 컴포넌트를 생성하고 네트워크 복제 설정
	Buff = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	Buff->SetIsReplicated(true);

	// 렉 보상 컴포넌트 생성
	LagCompensation = CreateDefaultSubobject<ULagCompensationComponent>(TEXT("LagCompensation"));

	// 캐릭터 움직임과 충돌 설정
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 850.f);

	// 기본 회전 상태 설정 및 네트워크 업데이트 빈도 설정
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	// 해체 시간라인 컴포넌트 생성
	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));

	// 첨부된 그레네이드를 생성하고 설정
	AttachedGrenade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Attached Grenade"));
	AttachedGrenade->SetupAttachment(GetMesh(), FName("GrenadeSocket"));
	AttachedGrenade->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 서버 측 리와인드를 위한 히트 박스 생성

	head = CreateDefaultSubobject<UBoxComponent>(TEXT("head"));
	head->SetupAttachment(GetMesh(), FName("head"));
	HitCollisionBoxes.Add(FName("head"), head);

	pelvis = CreateDefaultSubobject<UBoxComponent>(TEXT("pelvis"));
	pelvis->SetupAttachment(GetMesh(), FName("pelvis"));
	HitCollisionBoxes.Add(FName("pelvis"), pelvis);

	spine_02 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_02"));
	spine_02->SetupAttachment(GetMesh(), FName("spine_02"));
	HitCollisionBoxes.Add(FName("spine_02"), spine_02);

	spine_03 = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_03"));
	spine_03->SetupAttachment(GetMesh(), FName("spine_03"));
	HitCollisionBoxes.Add(FName("spine_03"), spine_03);

	upperarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_l"));
	upperarm_l->SetupAttachment(GetMesh(), FName("upperarm_l"));
	HitCollisionBoxes.Add(FName("upperarm_l"), upperarm_l);

	upperarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("upperarm_r"));
	upperarm_r->SetupAttachment(GetMesh(), FName("upperarm_r"));
	HitCollisionBoxes.Add(FName("upperarm_r"), upperarm_r);

	lowerarm_l = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_l"));
	lowerarm_l->SetupAttachment(GetMesh(), FName("lowerarm_l"));
	HitCollisionBoxes.Add(FName("lowerarm_l"), lowerarm_l);

	lowerarm_r = CreateDefaultSubobject<UBoxComponent>(TEXT("lowerarm_r"));
	lowerarm_r->SetupAttachment(GetMesh(), FName("lowerarm_r"));
	HitCollisionBoxes.Add(FName("lowerarm_r"), lowerarm_r);

	hand_l = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_l"));
	hand_l->SetupAttachment(GetMesh(), FName("hand_l"));
	HitCollisionBoxes.Add(FName("hand_l"), hand_l);

	hand_r = CreateDefaultSubobject<UBoxComponent>(TEXT("hand_r"));
	hand_r->SetupAttachment(GetMesh(), FName("hand_r"));
	HitCollisionBoxes.Add(FName("hand_r"), hand_r);

	blanket = CreateDefaultSubobject<UBoxComponent>(TEXT("blanket"));
	blanket->SetupAttachment(GetMesh(), FName("backpack"));
	HitCollisionBoxes.Add(FName("blanket"), blanket);

	backpack = CreateDefaultSubobject<UBoxComponent>(TEXT("backpack"));
	backpack->SetupAttachment(GetMesh(), FName("backpack"));
	HitCollisionBoxes.Add(FName("backpack"), backpack);

	thigh_l = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_l"));
	thigh_l->SetupAttachment(GetMesh(), FName("thigh_l"));
	HitCollisionBoxes.Add(FName("thigh_l"), thigh_l);

	thigh_r = CreateDefaultSubobject<UBoxComponent>(TEXT("thigh_r"));
	thigh_r->SetupAttachment(GetMesh(), FName("thigh_r"));
	HitCollisionBoxes.Add(FName("thigh_r"), thigh_r);

	calf_l = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_l"));
	calf_l->SetupAttachment(GetMesh(), FName("calf_l"));
	HitCollisionBoxes.Add(FName("calf_l"), calf_l);

	calf_r = CreateDefaultSubobject<UBoxComponent>(TEXT("calf_r"));
	calf_r->SetupAttachment(GetMesh(), FName("calf_r"));
	HitCollisionBoxes.Add(FName("calf_r"), calf_r);

	foot_l = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_l"));
	foot_l->SetupAttachment(GetMesh(), FName("foot_l"));
	HitCollisionBoxes.Add(FName("foot_l"), foot_l);

	foot_r = CreateDefaultSubobject<UBoxComponent>(TEXT("foot_r"));
	foot_r->SetupAttachment(GetMesh(), FName("foot_r"));
	HitCollisionBoxes.Add(FName("foot_r"), foot_r);

	// 모든 히트 박스에 대해 충돌 유형 및 응답 설정
	for (auto Box : HitCollisionBoxes)
	{
		if (Box.Value)
		{
			Box.Value->SetCollisionObjectType(ECC_HitBox);
			Box.Value->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
			Box.Value->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);
			Box.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

// 수명 동안 복제되는 속성 설정
void ALetsShootCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ALetsShootCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(ALetsShootCharacter, Health);
	DOREPLIFETIME(ALetsShootCharacter, Shield);
	DOREPLIFETIME(ALetsShootCharacter, bDisableGameplay);
}

// 리플리케이션 이후의 이동을 처리
void ALetsShootCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	SimProxiesTurn();
	TimeSinceLastMovementReplication = 0.f;
}

// 캐릭터를 제거
void ALetsShootCharacter::Elim(bool bPlayerLeftGame)
{
	DropOrDestroyWeapons();
	MulticastElim(bPlayerLeftGame);
}

// 다중 환경에서 캐릭터 제거 구현
void ALetsShootCharacter::MulticastElim_Implementation(bool bPlayerLeftGame)
{
	bLeftGame = bPlayerLeftGame;
	if (LetsShootPlayerController)
	{
		LetsShootPlayerController->SetHUDWeaponAmmo(0);
	}
	bElimmed = true;
	PlayElimMontage();
	// Start dissolve effect
	if (DissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicDissolveMaterialInstance);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), 0.55f);
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Glow"), 200.f);
	}
	StartDissolve();

	// Disable character movement
	bDisableGameplay = true;
	GetCharacterMovement()->DisableMovement();
	if (Combat)
	{
		Combat->FireButtonPressed(false);
	}
	// Disable collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttachedGrenade->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Spawn elim bot
	if (ElimBotEffect)
	{
		FVector ElimBotSpawnPoint(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 200.f);
		ElimBotComponent = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ElimBotEffect,
			ElimBotSpawnPoint,
			GetActorRotation()
		);
	}
	if (ElimBotSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(
			this,
			ElimBotSound,
			GetActorLocation()
		);
	}
	bool bHideSniperScope = IsLocallyControlled() &&
		Combat &&
		Combat->bAiming &&
		Combat->EquippedWeapon &&
		Combat->EquippedWeapon->GetWeaponType() == EWeaponType::EWT_SniperRifle;
	if (bHideSniperScope)
	{
		ShowSniperScopeWidget(false);
	}
	if (CrownComponent)
	{
		CrownComponent->DestroyComponent();
	}
	GetWorldTimerManager().SetTimer(
		ElimTimer,
		this,
		&ALetsShootCharacter::ElimTimerFinished,
		ElimDelay
	);
}

void ALetsShootCharacter::ElimTimerFinished()
{
	// 블래스터 게임 모드를 설정하거나 기존 블래스터 게임 모드를 찾습니다.
	LetsShootGameMode = LetsShootGameMode == nullptr ? GetWorld()->GetAuthGameMode<ALetsShootGameMode>() : LetsShootGameMode;
	// 게임 모드가 존재하고, 플레이어가 게임을 떠나지 않은 경우
	if (LetsShootGameMode && !bLeftGame)
	{
		// 리스폰을 요청합니다.
		LetsShootGameMode->RequestRespawn(this, Controller);
	}
	// 플레이어가 게임을 떠난 경우 및 로컬로 제어되는 경우
	if (bLeftGame && IsLocallyControlled())
	{
		// 게임을 떠났음을 방송합니다.
		OnLeftGame.Broadcast();
	}
}

void ALetsShootCharacter::ServerLeaveGame_Implementation()
{
	// 블래스터 게임 모드를 설정하거나 기존 블래스터 게임 모드를 찾습니다.
	LetsShootGameMode = LetsShootGameMode == nullptr ? GetWorld()->GetAuthGameMode<ALetsShootGameMode>() : LetsShootGameMode;
	// 블래스터 플레이어 상태를 설정하거나 기존 블래스터 플레이어 상태를 찾습니다.
	LetsShootPlayerState = LetsShootPlayerState == nullptr ? GetPlayerState<ALetsShootPlayerState>() : LetsShootPlayerState;
	// 게임 모드와 플레이어 상태가 존재하는 경우
	if (LetsShootGameMode && LetsShootPlayerState)
	{
		// 플레이어가 게임을 떠난 것을 알립니다.
		LetsShootGameMode->PlayerLeftGame(LetsShootPlayerState);
	}
}

void ALetsShootCharacter::DropOrDestroyWeapon(AWeapon* Weapon)
{
	// 무기가 없는 경우 반환
	if (Weapon == nullptr) return;
	// 무기를 파괴해야 하는 경우
	if (Weapon->bDestroyWeapon)
	{
		// 무기를 파괴
		Weapon->Destroy();
	}
	else
	{
		// 무기를 떨어뜨림
		Weapon->Dropped();
	}
}

void ALetsShootCharacter::DropOrDestroyWeapons()
{
	// 전투가 있는 경우
	if (Combat)
	{
		// 장착된 무기를 떨어뜨리거나 파괴
		if (Combat->EquippedWeapon)
		{
			DropOrDestroyWeapon(Combat->EquippedWeapon);
		}
		// 보조 무기를 떨어뜨리거나 파괴
		if (Combat->SecondaryWeapon)
		{
			DropOrDestroyWeapon(Combat->SecondaryWeapon);
		}
		// 깃발이 있는 경우 깃발을 떨어뜨림
		if (Combat->TheFlag)
		{
			Combat->TheFlag->Dropped();
		}
	}
}

void ALetsShootCharacter::OnPlayerStateInitialized()
{
	// 점수와 패배 횟수를 초기화
	LetsShootPlayerState->AddToScore(0.f);
	LetsShootPlayerState->AddToDefeats(0);
	// 팀 색상을 설정
	SetTeamColor(LetsShootPlayerState->GetTeam());
	// 스폰 위치를 설정
	SetSpawnPoint();
}

void ALetsShootCharacter::SetSpawnPoint()
{
	// 권한이 있고, 팀이 설정되어 있는 경우
	if (HasAuthority() && LetsShootPlayerState->GetTeam() != ETeam::ET_NoTeam)
	{
		// 모든 플레이어 시작점을 가져옴
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, ATeamPlayerStart::StaticClass(), PlayerStarts);
		TArray<ATeamPlayerStart*> TeamPlayerStarts;
		// 플레이어 시작점에서 같은 팀의 시작점을 찾음
		for (auto Start : PlayerStarts)
		{
			ATeamPlayerStart* TeamStart = Cast<ATeamPlayerStart>(Start);
			if (TeamStart && TeamStart->Team == LetsShootPlayerState->GetTeam())
			{
				TeamPlayerStarts.Add(TeamStart);
			}
		}
		// 팀 시작점이 하나 이상 있는 경우
		if (TeamPlayerStarts.Num() > 0)
		{
			// 무작위로 팀 시작점을 선택
			ATeamPlayerStart* ChosenPlayerStart = TeamPlayerStarts[FMath::RandRange(0, TeamPlayerStarts.Num() - 1)];
			// 선택된 시작점의 위치와 회전으로 액터의 위치와 회전을 설정
			SetActorLocationAndRotation(
				ChosenPlayerStart->GetActorLocation(),
				ChosenPlayerStart->GetActorRotation()
			);
		}
	}
}

void ALetsShootCharacter::Destroyed()
{
	Super::Destroyed();

	// ElimBotComponent가 있는 경우 파괴
	if (ElimBotComponent)
	{
		ElimBotComponent->DestroyComponent();
	}

	// 블래스터 게임 모드를 설정하거나 기존 블래스터 게임 모드를 찾습니다.
	LetsShootGameMode = LetsShootGameMode == nullptr ? GetWorld()->GetAuthGameMode<ALetsShootGameMode>() : LetsShootGameMode;
	// 경기가 진행중이 아닌지 확인
	bool bMatchNotInProgress = LetsShootGameMode && LetsShootGameMode->GetMatchState() != MatchState::InProgress;
	// 전투가 있고, 무기가 장착되어 있으며, 경기가 진행중이 아닌 경우
	if (Combat && Combat->EquippedWeapon && bMatchNotInProgress)
	{
		// 무기를 파괴
		Combat->EquippedWeapon->Destroy();
	}
}


// 멀티캐스트로 리드 상태 획득을 구현한 메서드
void ALetsShootCharacter::MulticastGainedTheLead_Implementation()
{
	// CrownSystem이 없는 경우 리턴
	if (CrownSystem == nullptr) return;
	// CrownComponent가 없는 경우
	if (CrownComponent == nullptr)
	{
		// CrownComponent를 생성
		CrownComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			CrownSystem,
			GetMesh(),
			FName(),
			GetActorLocation() + FVector(0.f, 0.f, 110.f),
			GetActorRotation(),
			EAttachLocation::KeepWorldPosition,
			false
		);
	}
	// CrownComponent가 있는 경우
	if (CrownComponent)
	{
		// CrownComponent를 활성화
		CrownComponent->Activate();
	}
}

// 멀티캐스트로 리드 상태 상실을 구현한 메서드
void ALetsShootCharacter::MulticastLostTheLead_Implementation()
{
	// CrownComponent가 있는 경우
	if (CrownComponent)
	{
		// CrownComponent를 파괴
		CrownComponent->DestroyComponent();
	}
}

// 팀 색상을 설정하는 메서드
void ALetsShootCharacter::SetTeamColor(ETeam Team)
{
	// 메시 또는 원래의 재질이 없는 경우 리턴
	if (GetMesh() == nullptr || OriginalMaterial == nullptr) return;
	switch (Team)
	{
	case ETeam::ET_NoTeam:  // 팀이 없는 경우
		GetMesh()->SetMaterial(0, OriginalMaterial);
		DissolveMaterialInstance = BlueDissolveMatInst;
		break;
	case ETeam::ET_BlueTeam:  // 팀이 Blue인 경우
		GetMesh()->SetMaterial(0, BlueMaterial);
		DissolveMaterialInstance = BlueDissolveMatInst;
		break;
	case ETeam::ET_RedTeam:  // 팀이 Red인 경우
		GetMesh()->SetMaterial(0, RedMaterial);
		DissolveMaterialInstance = RedDissolveMatInst;
		break;
	}
}

// 플레이 시작 시 호출되는 메서드
void ALetsShootCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 권한이 있는 경우
	if (HasAuthority())
	{
		// 데미지를 받을 때 호출될 메서드를 등록
		OnTakeAnyDamage.AddDynamic(this, &ALetsShootCharacter::ReceiveDamage);
	}
	// Grenade가 첨부된 경우
	if (AttachedGrenade)
	{
		// Grenade의 가시성을 false로 설정
		AttachedGrenade->SetVisibility(false);
	}
}

// 매 프레임마다 호출되는 메서드
void ALetsShootCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 자리에서 회전
	RotateInPlace(DeltaTime);
	// 캐릭터가 가까운 경우 카메라 숨김
	HideCameraIfCharacterClose();
	// 초기화 조사
	PollInit();
}


// 자리에서 회전하는 메서드
void ALetsShootCharacter::RotateInPlace(float DeltaTime)
{
	// 플래그를 들고 있을 경우
	if (Combat && Combat->bHoldingTheFlag)
	{
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
	// 무기를 장착한 경우
	if (Combat && Combat->EquippedWeapon) GetCharacterMovement()->bOrientRotationToMovement = false;
	if (Combat && Combat->EquippedWeapon) bUseControllerRotationYaw = true;
	// 게임 플레이가 비활성화된 경우
	if (bDisableGameplay)
	{
		bUseControllerRotationYaw = false;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
	if (GetLocalRole() > ENetRole::ROLE_SimulatedProxy && IsLocallyControlled())
	{
		AimOffset(DeltaTime);
	}
	else
	{
		// 이동 복제에 따른 시간 경과
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.25f)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();
	}
}

// 플레이어 입력 컴포넌트 설정 메서드
void ALetsShootCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ALetsShootCharacter::Jump);

	PlayerInputComponent->BindAxis("MoveForward", this, &ALetsShootCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ALetsShootCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &ALetsShootCharacter::Turn);
	PlayerInputComponent->BindAxis("LookUp", this, &ALetsShootCharacter::LookUp);

	PlayerInputComponent->BindAction("Equip", IE_Pressed, this, &ALetsShootCharacter::EquipButtonPressed);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ALetsShootCharacter::CrouchButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &ALetsShootCharacter::AimButtonPressed);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &ALetsShootCharacter::AimButtonReleased);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ALetsShootCharacter::FireButtonPressed);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ALetsShootCharacter::FireButtonReleased);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ALetsShootCharacter::ReloadButtonPressed);
	PlayerInputComponent->BindAction("ThrowGrenade", IE_Pressed, this, &ALetsShootCharacter::GrenadeButtonPressed);
}

// 컴포넌트 초기화 후 메서드
void ALetsShootCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (Combat)
	{
		Combat->Character = this;
	}
	if (Buff)
	{
		Buff->Character = this;
		// 초기 속도 설정
		Buff->SetInitialSpeeds(
			GetCharacterMovement()->MaxWalkSpeed,
			GetCharacterMovement()->MaxWalkSpeedCrouched
		);
		// 초기 점프 속도 설정
		Buff->SetInitialJumpVelocity(GetCharacterMovement()->JumpZVelocity);
	}
	if (LagCompensation)
	{
		LagCompensation->Character = this;
		if (Controller)
		{
			LagCompensation->Controller = Cast<ALetsShootPlayerController>(Controller);
		}
	}
}

// 발사 모션 재생 메서드
void ALetsShootCharacter::PlayFireMontage(bool bAiming)
{
	// Combat이 없거나 무기가 없는 경우 리턴
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireWeaponMontage)
	{
		AnimInstance->Montage_Play(FireWeaponMontage);
		FName SectionName;
		SectionName = bAiming ? FName("RifleAim") : FName("RifleHip");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}


// 재장전 모션 재생 메서드
void ALetsShootCharacter::PlayReloadMontage()
{
	// Combat이 없거나 무기가 없는 경우 함수 종료
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName;

		// 장착된 무기의 종류에 따라 섹션 이름 변경
		switch (Combat->EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
			SectionName = FName("Rifle");
			break;
		case EWeaponType::EWT_RocketLauncher:
			SectionName = FName("RocketLauncher");
			break;
		case EWeaponType::EWT_Pistol:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_SubmachineGun:
			SectionName = FName("Pistol");
			break;
		case EWeaponType::EWT_Shotgun:
			SectionName = FName("Shotgun");
			break;
		case EWeaponType::EWT_SniperRifle:
			SectionName = FName("SniperRifle");
			break;
		case EWeaponType::EWT_GrenadeLauncher:
			SectionName = FName("GrenadeLauncher");
			break;
		}

		// 특정 섹션으로 이동
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

// 처치 모션 재생 메서드
void ALetsShootCharacter::PlayElimMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ElimMontage)
	{
		AnimInstance->Montage_Play(ElimMontage);
	}
}

// 수류탄 투척 모션 재생 메서드
void ALetsShootCharacter::PlayThrowGrenadeMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ThrowGrenadeMontage)
	{
		AnimInstance->Montage_Play(ThrowGrenadeMontage);
	}
}

// 교체 모션 재생 메서드
void ALetsShootCharacter::PlaySwapMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && SwapMontage)
	{
		AnimInstance->Montage_Play(SwapMontage);
	}
}

// 피격 반응 모션 재생 메서드
void ALetsShootCharacter::PlayHitReactMontage()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		FName SectionName("FromFront");
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

// 수류탄 버튼이 눌릴 때의 처리 메서드
void ALetsShootCharacter::GrenadeButtonPressed()
{
	if (Combat)
	{
		// 플래그를 들고 있는 경우 수류탄을 던지지 않음
		if (Combat->bHoldingTheFlag) return;
		Combat->ThrowGrenade();
	}
}


// 메서드: ReceiveDamage
// 역할: 데미지를 받았을 때 호출되는 함수입니다.
void ALetsShootCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	LetsShootGameMode = LetsShootGameMode == nullptr ? GetWorld()->GetAuthGameMode<ALetsShootGameMode>() : LetsShootGameMode;
	if (bElimmed || LetsShootGameMode == nullptr) return;

	// 데미지 계산
	Damage = LetsShootGameMode->CalculateDamage(InstigatorController, Controller, Damage);

	float DamageToHealth = Damage;

	// 실드가 있을 경우 실드로 데미지를 막음
	if (Shield > 0.f)
	{
		if (Shield >= Damage)
		{
			Shield = FMath::Clamp(Shield - Damage, 0.f, MaxShield);
			DamageToHealth = 0.f;
		}
		else
		{
			DamageToHealth = FMath::Clamp(DamageToHealth - Shield, 0.f, Damage);
			Shield = 0.f;
		}
	}

	// 체력 업데이트
	Health = FMath::Clamp(Health - DamageToHealth, 0.f, MaxHealth);

	// HUD 업데이트
	UpdateHUDHealth();
	UpdateHUDShield();

	// 피격 반응 Montage 재생
	PlayHitReactMontage();

	// 체력이 0이면 플레이어 제거
	if (Health == 0.f)
	{
		if (LetsShootGameMode)
		{
			// 플레이어 컨트롤러와 공격자 컨트롤러를 가져옴
			LetsShootPlayerController = LetsShootPlayerController == nullptr ? Cast<ALetsShootPlayerController>(Controller) : LetsShootPlayerController;
			ALetsShootPlayerController* AttackerController = Cast<ALetsShootPlayerController>(InstigatorController);

			// 게임 모드에 플레이어 제거를 알림
			LetsShootGameMode->PlayerEliminated(this, LetsShootPlayerController, AttackerController);
		}
	}
}

// 메서드: MoveForward
// 역할: 앞으로 이동하는 함수입니다.
void ALetsShootCharacter::MoveForward(float Value)
{
	if (bDisableGameplay) return;

	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));

		// 이동 입력을 적용
		AddMovementInput(Direction, Value);
	}
}

// 메서드: MoveRight
// 역할: 오른쪽으로 이동하는 함수입니다.
void ALetsShootCharacter::MoveRight(float Value)
{
	if (bDisableGameplay) return;

	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));

		// 이동 입력을 적용
		AddMovementInput(Direction, Value);
	}
}

// 메서드: Turn
// 역할: 캐릭터의 좌우 회전을 처리하는 함수입니다.
void ALetsShootCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

// 메서드: LookUp
// 역할: 캐릭터의 상하 시선을 처리하는 함수입니다.
void ALetsShootCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

// 메서드: EquipButtonPressed
// 역할: 장비 버튼을 눌렀을 때 호출되는 함수입니다.
void ALetsShootCharacter::EquipButtonPressed()
{
	if (bDisableGameplay) return;

	if (Combat)
	{
		// 깃발을 들고 있는 경우 무시
		if (Combat->bHoldingTheFlag) return;

		// 전투 상태가 미점령 상태인 경우에만 실행
		if (Combat->CombatState == ECombatState::ECS_Unoccupied) ServerEquipButtonPressed();

		// 무기를 교체해야 하는지 확인하고, 서버에서 실행
		bool bSwap = Combat->ShouldSwapWeapons() &&
			!HasAuthority() &&
			Combat->CombatState == ECombatState::ECS_Unoccupied &&
			OverlappingWeapon == nullptr;

		if (bSwap)
		{
			// 교체 Montage 재생
			PlaySwapMontage();

			// 전투 상태 변경 및 교체 완료 여부 초기화
			Combat->CombatState = ECombatState::ECS_SwappingWeapons;
			bFinishedSwapping = false;
		}
	}
}


// 메서드: ServerEquipButtonPressed_Implementation
// 역할: 서버에서 실행되는 장비 버튼 눌림 함수입니다.
void ALetsShootCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat)
	{
		if (OverlappingWeapon)
		{
			// 겹치는 무기가 있을 경우 무기를 장착합니다.
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else if (Combat->ShouldSwapWeapons())
		{
			// 무기를 교체해야 하는 경우 무기를 교체합니다.
			Combat->SwapWeapons();
		}
	}
}

// 메서드: CrouchButtonPressed
// 역할: 앉기 버튼을 눌렀을 때 호출되는 함수입니다.
void ALetsShootCharacter::CrouchButtonPressed()
{
	if (Combat && Combat->bHoldingTheFlag) return;
	if (bDisableGameplay) return;
	if (bIsCrouched)
	{
		// 이미 앉아 있는 경우 서서히 일어나게 합니다.
		UnCrouch();
	}
	else
	{
		// 서있는 경우 서서히 앉게 합니다.
		Crouch();
	}
}

// 메서드: ReloadButtonPressed
// 역할: 재장전 버튼을 눌렀을 때 호출되는 함수입니다.
void ALetsShootCharacter::ReloadButtonPressed()
{
	if (Combat && Combat->bHoldingTheFlag) return;
	if (bDisableGameplay) return;
	if (Combat)
	{
		// 전투 객체의 재장전 함수를 호출합니다.
		Combat->Reload();
	}
}

// 메서드: AimButtonPressed
// 역할: 조준 버튼을 눌렀을 때 호출되는 함수입니다.
void ALetsShootCharacter::AimButtonPressed()
{
	if (Combat && Combat->bHoldingTheFlag) return;
	if (bDisableGameplay) return;
	if (Combat)
	{
		// 전투 객체의 조준 설정을 활성화합니다.
		Combat->SetAiming(true);
	}
}

// 메서드: AimButtonReleased
// 역할: 조준 버튼을 눌렀다 뗐을 때 호출되는 함수입니다.
void ALetsShootCharacter::AimButtonReleased()
{
	if (Combat && Combat->bHoldingTheFlag) return;
	if (bDisableGameplay) return;
	if (Combat)
	{
		// 전투 객체의 조준 설정을 비활성화합니다.
		Combat->SetAiming(false);
	}
}

// 메서드: CalculateSpeed
// 역할: 이동 속도를 계산하는 함수입니다.
float ALetsShootCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;

	// 이동 속도 계산
	return Velocity.Size();
}


// 메서드: AimOffset
// 역할: 조준 오프셋을 처리하는 함수입니다.
void ALetsShootCharacter::AimOffset(float DeltaTime)
{
	if (Combat && Combat->EquippedWeapon == nullptr) return;
	float Speed = CalculateSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	// 멈춰있고 점프 중이 아닌 경우
	if (Speed == 0.f && !bIsInAir)
	{
		bRotateRootBone = true;
		FRotator CurrentAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		bUseControllerRotationYaw = true;
		TurnInPlace(DeltaTime);
	}

	// 달리거나 공중에 있는 경우
	if (Speed > 0.f || bIsInAir)
	{
		bRotateRootBone = false;
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		bUseControllerRotationYaw = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}

	CalculateAO_Pitch();
}

// 메서드: CalculateAO_Pitch
// 역할: AO_Pitch 값을 계산하는 함수입니다.
void ALetsShootCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;

	// AO_Pitch가 90보다 크고 로컬 컨트롤이 아닌 경우
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		// pitch를 [270, 360) 범위에서 [-90, 0) 범위로 매핑합니다.
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

// 메서드: SimProxiesTurn
// 역할: 프록시 회전을 처리하는 함수입니다.
void ALetsShootCharacter::SimProxiesTurn()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;
	bRotateRootBone = false;
	float Speed = CalculateSpeed();

	// 이동 속도가 0보다 큰 경우
	if (Speed > 0.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}

	ProxyRotationLastFrame = ProxyRotation;
	ProxyRotation = GetActorRotation();
	ProxyYaw = UKismetMathLibrary::NormalizedDeltaRotator(ProxyRotation, ProxyRotationLastFrame).Yaw;

	if (FMath::Abs(ProxyYaw) > TurnThreshold)
	{
		// 프록시 Yaw 각도가 임계값보다 큰 경우
		if (ProxyYaw > TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Right;
		}
		else if (ProxyYaw < -TurnThreshold)
		{
			TurningInPlace = ETurningInPlace::ETIP_Left;
		}
		else
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		}
		return;
	}
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
}


// 메서드: Jump
// 역할: 점프 기능을 처리하는 함수입니다.
void ALetsShootCharacter::Jump()
{
	if (Combat && Combat->bHoldingTheFlag) return;
	if (bDisableGameplay) return;
	if (bIsCrouched)
	{
		// 앉아 있는 상태에서 점프하면 일어나게 합니다.
		UnCrouch();
	}
	else
	{
		// 일반적인 점프 동작을 실행합니다.
		Super::Jump();
	}
}

// 메서드: FireButtonPressed
// 역할: 발사 버튼을 눌렀을 때 호출되는 함수입니다.
void ALetsShootCharacter::FireButtonPressed()
{
	if (Combat && Combat->bHoldingTheFlag) return;
	if (bDisableGameplay) return;
	if (Combat)
	{
		// 전투 객체의 발사 버튼을 눌렀음을 전달합니다.
		Combat->FireButtonPressed(true);
	}
}

// 메서드: FireButtonReleased
// 역할: 발사 버튼을 눌렀다 뗐을 때 호출되는 함수입니다.
void ALetsShootCharacter::FireButtonReleased()
{
	if (Combat && Combat->bHoldingTheFlag) return;
	if (bDisableGameplay) return;
	if (Combat)
	{
		// 전투 객체의 발사 버튼을 떼었음을 전달합니다.
		Combat->FireButtonPressed(false);
	}
}

// 메서드: TurnInPlace
// 역할: 제자리에서 회전하는 동작을 처리하는 함수입니다.
void ALetsShootCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Right;
	}
	else if (AO_Yaw < -90.f)
	{
		TurningInPlace = ETurningInPlace::ETIP_Left;
	}
	if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 4.f);
		AO_Yaw = InterpAO_Yaw;
		if (FMath::Abs(AO_Yaw) < 15.f)
		{
			TurningInPlace = ETurningInPlace::ETIP_NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

// 메서드: HideCameraIfCharacterClose
// 역할: 캐릭터와 카메라의 거리에 따라 카메라와 무기의 가시성을 조절하는 함수입니다.
void ALetsShootCharacter::HideCameraIfCharacterClose()
{
	if (!IsLocallyControlled()) return;
	if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		// 캐릭터와 카메라의 거리가 임계값보다 작은 경우
		GetMesh()->SetVisibility(false);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
		if (Combat && Combat->SecondaryWeapon && Combat->SecondaryWeapon->GetWeaponMesh())
		{
			Combat->SecondaryWeapon->GetWeaponMesh()->bOwnerNoSee = true;
		}
	}
	else
	{
		// 캐릭터와 카메라의 거리가 임계값보다 큰 경우
		GetMesh()->SetVisibility(true);
		if (Combat && Combat->EquippedWeapon && Combat->EquippedWeapon->GetWeaponMesh())
		{
			Combat->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
		if (Combat && Combat->SecondaryWeapon && Combat->SecondaryWeapon->GetWeaponMesh())
		{
			Combat->SecondaryWeapon->GetWeaponMesh()->bOwnerNoSee = false;
		}
	}
}



// 메서드: OnRep_Health
// 역할: Health 값이 Replicate되었을 때 호출되는 함수입니다.
void ALetsShootCharacter::OnRep_Health(float LastHealth)
{
	UpdateHUDHealth();
	if (Health < LastHealth)
	{
		// Health가 감소한 경우 피격 반응 Montage를 재생합니다.
		PlayHitReactMontage();
	}
}

// 메서드: OnRep_Shield
// 역할: Shield 값이 Replicate되었을 때 호출되는 함수입니다.
void ALetsShootCharacter::OnRep_Shield(float LastShield)
{
	UpdateHUDShield();
	if (Shield < LastShield)
	{
		// Shield가 감소한 경우 피격 반응 Montage를 재생합니다.
		PlayHitReactMontage();
	}
}

// 메서드: UpdateHUDHealth
// 역할: HUD의 Health 정보를 업데이트하는 함수입니다.
void ALetsShootCharacter::UpdateHUDHealth()
{
	LetsShootPlayerController = LetsShootPlayerController == nullptr ? Cast<ALetsShootPlayerController>(Controller) : LetsShootPlayerController;
	if (LetsShootPlayerController)
	{
		LetsShootPlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

// 메서드: UpdateHUDShield
// 역할: HUD의 Shield 정보를 업데이트하는 함수입니다.
void ALetsShootCharacter::UpdateHUDShield()
{
	LetsShootPlayerController = LetsShootPlayerController == nullptr ? Cast<ALetsShootPlayerController>(Controller) : LetsShootPlayerController;
	if (LetsShootPlayerController)
	{
		LetsShootPlayerController->SetHUDShield(Shield, MaxShield);
	}
}

// 메서드: UpdateHUDAmmo
// 역할: HUD의 탄약 정보를 업데이트하는 함수입니다.
void ALetsShootCharacter::UpdateHUDAmmo()
{
	LetsShootPlayerController = LetsShootPlayerController == nullptr ? Cast<ALetsShootPlayerController>(Controller) : LetsShootPlayerController;
	if (LetsShootPlayerController && Combat && Combat->EquippedWeapon)
	{
		LetsShootPlayerController->SetHUDCarriedAmmo(Combat->CarriedAmmo);
		LetsShootPlayerController->SetHUDWeaponAmmo(Combat->EquippedWeapon->GetAmmo());
	}
}

// 메서드: SpawnDefaultWeapon
// 역할: 기본 무기를 스폰하는 함수입니다.
void ALetsShootCharacter::SpawnDefaultWeapon()
{
	LetsShootGameMode = LetsShootGameMode == nullptr ? GetWorld()->GetAuthGameMode<ALetsShootGameMode>() : LetsShootGameMode;
	UWorld* World = GetWorld();
	if (LetsShootGameMode && World && !bElimmed && DefaultWeaponClass)
	{
		// 기본 무기를 스폰하여 Equip합니다.
		AWeapon* StartingWeapon = World->SpawnActor<AWeapon>(DefaultWeaponClass);
		StartingWeapon->bDestroyWeapon = true;
		if (Combat)
		{
			Combat->EquipWeapon(StartingWeapon);
		}
	}
}



// 메서드: PollInit
// 역할: 초기화 작업을 수행하는 함수입니다.
void ALetsShootCharacter::PollInit()
{
	if (LetsShootPlayerState == nullptr)
	{
		// LetsShootPlayerState가 nullptr인 경우 PlayerState를 가져와서 초기화합니다.
		LetsShootPlayerState = GetPlayerState<ALetsShootPlayerState>();
		if (LetsShootPlayerState)
		{
			OnPlayerStateInitialized();

			ALetsShootGameState* LetsShootGameState = Cast<ALetsShootGameState>(UGameplayStatics::GetGameState(this));

			if (LetsShootGameState && LetsShootGameState->TopScoringPlayers.Contains(LetsShootPlayerState))
			{
				MulticastGainedTheLead();
			}
		}
	}
	if (LetsShootPlayerController == nullptr)
	{
		LetsShootPlayerController = LetsShootPlayerController == nullptr ? Cast<ALetsShootPlayerController>(Controller) : LetsShootPlayerController;
		if (LetsShootPlayerController)
		{
			SpawDefaultWeapon();
			UpdateHUDAmmo();
			UpdateHUDHealth();
			UpdateHUDShield();
		}
	}
}

// 메서드: UpdateDissolveMaterial
// 역할: Dissolve 재질을 업데이트하는 함수입니다.
void ALetsShootCharacter::UpdateDissolveMaterial(float DissolveValue)
{
	if (DynamicDissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	}
}

// 메서드: StartDissolve
// 역할: Dissolve 효과를 시작하는 함수입니다.
void ALetsShootCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &ALetsShootCharacter::UpdateDissolveMaterial);
	if (DissolveCurve && DissolveTimeline)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeline->Play();
	}
}

// 메서드: SetOverlappingWeapon
// 역할: OverlappingWeapon을 설정하는 함수입니다.
void ALetsShootCharacter::SetOverlappingWeapon(AWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	OverlappingWeapon = Weapon;
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}

// 메서드: OnRep_OverlappingWeapon
// 역할: OverlappingWeapon이 Replicate되었을 때 호출되는 함수입니다.
void ALetsShootCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}



// 메서드: IsWeaponEquipped
// 역할: 무기가 장착되어 있는지 여부를 반환하는 함수입니다.
bool ALetsShootCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

// 메서드: IsAiming
// 역할: 조준 중인지 여부를 반환하는 함수입니다.
bool ALetsShootCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);
}

// 메서드: GetEquippedWeapon
// 역할: 현재 장착된 무기를 반환하는 함수입니다.
AWeapon* ALetsShootCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr) return nullptr;
	return Combat->EquippedWeapon;
}

// 메서드: GetHitTarget
// 역할: 공격 대상의 위치를 반환하는 함수입니다.
FVector ALetsShootCharacter::GetHitTarget() const
{
	if (Combat == nullptr) return FVector();
	return Combat->HitTarget;
}

// 메서드: GetCombatState
// 역할: 전투 상태를 반환하는 함수입니다.
ECombatState ALetsShootCharacter::GetCombatState() const
{
	if (Combat == nullptr) return ECombatState::ECS_MAX;
	return Combat->CombatState;
}

// 메서드: IsLocallyReloading
// 역할: 로컬에서 재장전 중인지 여부를 반환하는 함수입니다.
bool ALetsShootCharacter::IsLocallyReloading()
{
	if (Combat == nullptr) return false;
	return Combat->bLocallyReloading;
}

// 메서드: IsHoldingTheFlag
// 역할: 깃발을 들고 있는지 여부를 반환하는 함수입니다.
bool ALetsShootCharacter::IsHoldingTheFlag() const
{
	if (Combat == nullptr) return false;
	return Combat->bHoldingTheFlag;
}

// 메서드: GetTeam
// 역할: 플레이어의 소속 팀을 반환하는 함수입니다.
ETeam ALetsShootCharacter::GetTeam()
{
	LetsShootPlayerState = LetsShootPlayerState == nullptr ? GetPlayerState<ALetsShootPlayerState>() : LetsShootPlayerState;
	if (LetsShootPlayerState == nullptr) return ETeam::ET_NoTeam;
	return LetsShootPlayerState->GetTeam();
}

// 메서드: SetHoldingTheFlag
// 역할: 깃발을 들고 있는지 여부를 설정하는 함수입니다.
void ALetsShootCharacter::SetHoldingTheFlag(bool bHolding)
{
	if (Combat == nullptr) return;
	Combat->bHoldingTheFlag = bHolding;
}

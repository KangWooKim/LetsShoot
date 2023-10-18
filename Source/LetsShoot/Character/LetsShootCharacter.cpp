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
	// ƽ�� �۵��ϵ��� ����
	PrimaryActorTick.bCanEverTick = true;

	// ī�޶� ���� �����ϰ� ����
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->bUsePawnControlRotation = true;

	// ���� ī�޶� �����ϰ� ����
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// ��Ʈ�ѷ��� ȸ�� ������ ������� �ʵ��� ����
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// ������� ���� ����
	OverheadWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("OverheadWidget"));
	OverheadWidget->SetupAttachment(RootComponent);

	// �Ĺ� ������Ʈ�� �����ϰ� ��Ʈ��ũ ���� ����
	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat"));
	Combat->SetIsReplicated(true);

	// ���� ������Ʈ�� �����ϰ� ��Ʈ��ũ ���� ����
	Buff = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	Buff->SetIsReplicated(true);

	// �� ���� ������Ʈ ����
	LagCompensation = CreateDefaultSubobject<ULagCompensationComponent>(TEXT("LagCompensation"));

	// ĳ���� �����Ӱ� �浹 ����
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 850.f);

	// �⺻ ȸ�� ���� ���� �� ��Ʈ��ũ ������Ʈ �� ����
	TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.f;

	// ��ü �ð����� ������Ʈ ����
	DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DissolveTimelineComponent"));

	// ÷�ε� �׷����̵带 �����ϰ� ����
	AttachedGrenade = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Attached Grenade"));
	AttachedGrenade->SetupAttachment(GetMesh(), FName("GrenadeSocket"));
	AttachedGrenade->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// ���� �� �����ε带 ���� ��Ʈ �ڽ� ����

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

	// ��� ��Ʈ �ڽ��� ���� �浹 ���� �� ���� ����
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

// ���� ���� �����Ǵ� �Ӽ� ����
void ALetsShootCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ALetsShootCharacter, OverlappingWeapon, COND_OwnerOnly);
	DOREPLIFETIME(ALetsShootCharacter, Health);
	DOREPLIFETIME(ALetsShootCharacter, Shield);
	DOREPLIFETIME(ALetsShootCharacter, bDisableGameplay);
}

// ���ø����̼� ������ �̵��� ó��
void ALetsShootCharacter::OnRep_ReplicatedMovement()
{
	Super::OnRep_ReplicatedMovement();
	SimProxiesTurn();
	TimeSinceLastMovementReplication = 0.f;
}

// ĳ���͸� ����
void ALetsShootCharacter::Elim(bool bPlayerLeftGame)
{
	DropOrDestroyWeapons();
	MulticastElim(bPlayerLeftGame);
}

// ���� ȯ�濡�� ĳ���� ���� ����
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
	// ������ ���� ��带 �����ϰų� ���� ������ ���� ��带 ã���ϴ�.
	LetsShootGameMode = LetsShootGameMode == nullptr ? GetWorld()->GetAuthGameMode<ALetsShootGameMode>() : LetsShootGameMode;
	// ���� ��尡 �����ϰ�, �÷��̾ ������ ������ ���� ���
	if (LetsShootGameMode && !bLeftGame)
	{
		// �������� ��û�մϴ�.
		LetsShootGameMode->RequestRespawn(this, Controller);
	}
	// �÷��̾ ������ ���� ��� �� ���÷� ����Ǵ� ���
	if (bLeftGame && IsLocallyControlled())
	{
		// ������ �������� ����մϴ�.
		OnLeftGame.Broadcast();
	}
}

void ALetsShootCharacter::ServerLeaveGame_Implementation()
{
	// ������ ���� ��带 �����ϰų� ���� ������ ���� ��带 ã���ϴ�.
	LetsShootGameMode = LetsShootGameMode == nullptr ? GetWorld()->GetAuthGameMode<ALetsShootGameMode>() : LetsShootGameMode;
	// ������ �÷��̾� ���¸� �����ϰų� ���� ������ �÷��̾� ���¸� ã���ϴ�.
	LetsShootPlayerState = LetsShootPlayerState == nullptr ? GetPlayerState<ALetsShootPlayerState>() : LetsShootPlayerState;
	// ���� ���� �÷��̾� ���°� �����ϴ� ���
	if (LetsShootGameMode && LetsShootPlayerState)
	{
		// �÷��̾ ������ ���� ���� �˸��ϴ�.
		LetsShootGameMode->PlayerLeftGame(LetsShootPlayerState);
	}
}

void ALetsShootCharacter::DropOrDestroyWeapon(AWeapon* Weapon)
{
	// ���Ⱑ ���� ��� ��ȯ
	if (Weapon == nullptr) return;
	// ���⸦ �ı��ؾ� �ϴ� ���
	if (Weapon->bDestroyWeapon)
	{
		// ���⸦ �ı�
		Weapon->Destroy();
	}
	else
	{
		// ���⸦ ����߸�
		Weapon->Dropped();
	}
}

void ALetsShootCharacter::DropOrDestroyWeapons()
{
	// ������ �ִ� ���
	if (Combat)
	{
		// ������ ���⸦ ����߸��ų� �ı�
		if (Combat->EquippedWeapon)
		{
			DropOrDestroyWeapon(Combat->EquippedWeapon);
		}
		// ���� ���⸦ ����߸��ų� �ı�
		if (Combat->SecondaryWeapon)
		{
			DropOrDestroyWeapon(Combat->SecondaryWeapon);
		}
		// ����� �ִ� ��� ����� ����߸�
		if (Combat->TheFlag)
		{
			Combat->TheFlag->Dropped();
		}
	}
}

void ALetsShootCharacter::OnPlayerStateInitialized()
{
	// ������ �й� Ƚ���� �ʱ�ȭ
	LetsShootPlayerState->AddToScore(0.f);
	LetsShootPlayerState->AddToDefeats(0);
	// �� ������ ����
	SetTeamColor(LetsShootPlayerState->GetTeam());
	// ���� ��ġ�� ����
	SetSpawnPoint();
}

void ALetsShootCharacter::SetSpawnPoint()
{
	// ������ �ְ�, ���� �����Ǿ� �ִ� ���
	if (HasAuthority() && LetsShootPlayerState->GetTeam() != ETeam::ET_NoTeam)
	{
		// ��� �÷��̾� �������� ������
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, ATeamPlayerStart::StaticClass(), PlayerStarts);
		TArray<ATeamPlayerStart*> TeamPlayerStarts;
		// �÷��̾� ���������� ���� ���� �������� ã��
		for (auto Start : PlayerStarts)
		{
			ATeamPlayerStart* TeamStart = Cast<ATeamPlayerStart>(Start);
			if (TeamStart && TeamStart->Team == LetsShootPlayerState->GetTeam())
			{
				TeamPlayerStarts.Add(TeamStart);
			}
		}
		// �� �������� �ϳ� �̻� �ִ� ���
		if (TeamPlayerStarts.Num() > 0)
		{
			// �������� �� �������� ����
			ATeamPlayerStart* ChosenPlayerStart = TeamPlayerStarts[FMath::RandRange(0, TeamPlayerStarts.Num() - 1)];
			// ���õ� �������� ��ġ�� ȸ������ ������ ��ġ�� ȸ���� ����
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

	// ElimBotComponent�� �ִ� ��� �ı�
	if (ElimBotComponent)
	{
		ElimBotComponent->DestroyComponent();
	}

	// ������ ���� ��带 �����ϰų� ���� ������ ���� ��带 ã���ϴ�.
	LetsShootGameMode = LetsShootGameMode == nullptr ? GetWorld()->GetAuthGameMode<ALetsShootGameMode>() : LetsShootGameMode;
	// ��Ⱑ �������� �ƴ��� Ȯ��
	bool bMatchNotInProgress = LetsShootGameMode && LetsShootGameMode->GetMatchState() != MatchState::InProgress;
	// ������ �ְ�, ���Ⱑ �����Ǿ� ������, ��Ⱑ �������� �ƴ� ���
	if (Combat && Combat->EquippedWeapon && bMatchNotInProgress)
	{
		// ���⸦ �ı�
		Combat->EquippedWeapon->Destroy();
	}
}


// ��Ƽĳ��Ʈ�� ���� ���� ȹ���� ������ �޼���
void ALetsShootCharacter::MulticastGainedTheLead_Implementation()
{
	// CrownSystem�� ���� ��� ����
	if (CrownSystem == nullptr) return;
	// CrownComponent�� ���� ���
	if (CrownComponent == nullptr)
	{
		// CrownComponent�� ����
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
	// CrownComponent�� �ִ� ���
	if (CrownComponent)
	{
		// CrownComponent�� Ȱ��ȭ
		CrownComponent->Activate();
	}
}

// ��Ƽĳ��Ʈ�� ���� ���� ����� ������ �޼���
void ALetsShootCharacter::MulticastLostTheLead_Implementation()
{
	// CrownComponent�� �ִ� ���
	if (CrownComponent)
	{
		// CrownComponent�� �ı�
		CrownComponent->DestroyComponent();
	}
}

// �� ������ �����ϴ� �޼���
void ALetsShootCharacter::SetTeamColor(ETeam Team)
{
	// �޽� �Ǵ� ������ ������ ���� ��� ����
	if (GetMesh() == nullptr || OriginalMaterial == nullptr) return;
	switch (Team)
	{
	case ETeam::ET_NoTeam:  // ���� ���� ���
		GetMesh()->SetMaterial(0, OriginalMaterial);
		DissolveMaterialInstance = BlueDissolveMatInst;
		break;
	case ETeam::ET_BlueTeam:  // ���� Blue�� ���
		GetMesh()->SetMaterial(0, BlueMaterial);
		DissolveMaterialInstance = BlueDissolveMatInst;
		break;
	case ETeam::ET_RedTeam:  // ���� Red�� ���
		GetMesh()->SetMaterial(0, RedMaterial);
		DissolveMaterialInstance = RedDissolveMatInst;
		break;
	}
}

// �÷��� ���� �� ȣ��Ǵ� �޼���
void ALetsShootCharacter::BeginPlay()
{
	Super::BeginPlay();

	// ������ �ִ� ���
	if (HasAuthority())
	{
		// �������� ���� �� ȣ��� �޼��带 ���
		OnTakeAnyDamage.AddDynamic(this, &ALetsShootCharacter::ReceiveDamage);
	}
	// Grenade�� ÷�ε� ���
	if (AttachedGrenade)
	{
		// Grenade�� ���ü��� false�� ����
		AttachedGrenade->SetVisibility(false);
	}
}

// �� �����Ӹ��� ȣ��Ǵ� �޼���
void ALetsShootCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// �ڸ����� ȸ��
	RotateInPlace(DeltaTime);
	// ĳ���Ͱ� ����� ��� ī�޶� ����
	HideCameraIfCharacterClose();
	// �ʱ�ȭ ����
	PollInit();
}


// �ڸ����� ȸ���ϴ� �޼���
void ALetsShootCharacter::RotateInPlace(float DeltaTime)
{
	// �÷��׸� ��� ���� ���
	if (Combat && Combat->bHoldingTheFlag)
	{
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		TurningInPlace = ETurningInPlace::ETIP_NotTurning;
		return;
	}
	// ���⸦ ������ ���
	if (Combat && Combat->EquippedWeapon) GetCharacterMovement()->bOrientRotationToMovement = false;
	if (Combat && Combat->EquippedWeapon) bUseControllerRotationYaw = true;
	// ���� �÷��̰� ��Ȱ��ȭ�� ���
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
		// �̵� ������ ���� �ð� ���
		TimeSinceLastMovementReplication += DeltaTime;
		if (TimeSinceLastMovementReplication > 0.25f)
		{
			OnRep_ReplicatedMovement();
		}
		CalculateAO_Pitch();
	}
}

// �÷��̾� �Է� ������Ʈ ���� �޼���
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

// ������Ʈ �ʱ�ȭ �� �޼���
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
		// �ʱ� �ӵ� ����
		Buff->SetInitialSpeeds(
			GetCharacterMovement()->MaxWalkSpeed,
			GetCharacterMovement()->MaxWalkSpeedCrouched
		);
		// �ʱ� ���� �ӵ� ����
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

// �߻� ��� ��� �޼���
void ALetsShootCharacter::PlayFireMontage(bool bAiming)
{
	// Combat�� ���ų� ���Ⱑ ���� ��� ����
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


// ������ ��� ��� �޼���
void ALetsShootCharacter::PlayReloadMontage()
{
	// Combat�� ���ų� ���Ⱑ ���� ��� �Լ� ����
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectionName;

		// ������ ������ ������ ���� ���� �̸� ����
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

		// Ư�� �������� �̵�
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}

// óġ ��� ��� �޼���
void ALetsShootCharacter::PlayElimMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ElimMontage)
	{
		AnimInstance->Montage_Play(ElimMontage);
	}
}

// ����ź ��ô ��� ��� �޼���
void ALetsShootCharacter::PlayThrowGrenadeMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ThrowGrenadeMontage)
	{
		AnimInstance->Montage_Play(ThrowGrenadeMontage);
	}
}

// ��ü ��� ��� �޼���
void ALetsShootCharacter::PlaySwapMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && SwapMontage)
	{
		AnimInstance->Montage_Play(SwapMontage);
	}
}

// �ǰ� ���� ��� ��� �޼���
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

// ����ź ��ư�� ���� ���� ó�� �޼���
void ALetsShootCharacter::GrenadeButtonPressed()
{
	if (Combat)
	{
		// �÷��׸� ��� �ִ� ��� ����ź�� ������ ����
		if (Combat->bHoldingTheFlag) return;
		Combat->ThrowGrenade();
	}
}


// �޼���: ReceiveDamage
// ����: �������� �޾��� �� ȣ��Ǵ� �Լ��Դϴ�.
void ALetsShootCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	LetsShootGameMode = LetsShootGameMode == nullptr ? GetWorld()->GetAuthGameMode<ALetsShootGameMode>() : LetsShootGameMode;
	if (bElimmed || LetsShootGameMode == nullptr) return;

	// ������ ���
	Damage = LetsShootGameMode->CalculateDamage(InstigatorController, Controller, Damage);

	float DamageToHealth = Damage;

	// �ǵ尡 ���� ��� �ǵ�� �������� ����
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

	// ü�� ������Ʈ
	Health = FMath::Clamp(Health - DamageToHealth, 0.f, MaxHealth);

	// HUD ������Ʈ
	UpdateHUDHealth();
	UpdateHUDShield();

	// �ǰ� ���� Montage ���
	PlayHitReactMontage();

	// ü���� 0�̸� �÷��̾� ����
	if (Health == 0.f)
	{
		if (LetsShootGameMode)
		{
			// �÷��̾� ��Ʈ�ѷ��� ������ ��Ʈ�ѷ��� ������
			LetsShootPlayerController = LetsShootPlayerController == nullptr ? Cast<ALetsShootPlayerController>(Controller) : LetsShootPlayerController;
			ALetsShootPlayerController* AttackerController = Cast<ALetsShootPlayerController>(InstigatorController);

			// ���� ��忡 �÷��̾� ���Ÿ� �˸�
			LetsShootGameMode->PlayerEliminated(this, LetsShootPlayerController, AttackerController);
		}
	}
}

// �޼���: MoveForward
// ����: ������ �̵��ϴ� �Լ��Դϴ�.
void ALetsShootCharacter::MoveForward(float Value)
{
	if (bDisableGameplay) return;

	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X));

		// �̵� �Է��� ����
		AddMovementInput(Direction, Value);
	}
}

// �޼���: MoveRight
// ����: ���������� �̵��ϴ� �Լ��Դϴ�.
void ALetsShootCharacter::MoveRight(float Value)
{
	if (bDisableGameplay) return;

	if (Controller != nullptr && Value != 0.f)
	{
		const FRotator YawRotation(0.f, Controller->GetControlRotation().Yaw, 0.f);
		const FVector Direction(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y));

		// �̵� �Է��� ����
		AddMovementInput(Direction, Value);
	}
}

// �޼���: Turn
// ����: ĳ������ �¿� ȸ���� ó���ϴ� �Լ��Դϴ�.
void ALetsShootCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

// �޼���: LookUp
// ����: ĳ������ ���� �ü��� ó���ϴ� �Լ��Դϴ�.
void ALetsShootCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

// �޼���: EquipButtonPressed
// ����: ��� ��ư�� ������ �� ȣ��Ǵ� �Լ��Դϴ�.
void ALetsShootCharacter::EquipButtonPressed()
{
	if (bDisableGameplay) return;

	if (Combat)
	{
		// ����� ��� �ִ� ��� ����
		if (Combat->bHoldingTheFlag) return;

		// ���� ���°� ������ ������ ��쿡�� ����
		if (Combat->CombatState == ECombatState::ECS_Unoccupied) ServerEquipButtonPressed();

		// ���⸦ ��ü�ؾ� �ϴ��� Ȯ���ϰ�, �������� ����
		bool bSwap = Combat->ShouldSwapWeapons() &&
			!HasAuthority() &&
			Combat->CombatState == ECombatState::ECS_Unoccupied &&
			OverlappingWeapon == nullptr;

		if (bSwap)
		{
			// ��ü Montage ���
			PlaySwapMontage();

			// ���� ���� ���� �� ��ü �Ϸ� ���� �ʱ�ȭ
			Combat->CombatState = ECombatState::ECS_SwappingWeapons;
			bFinishedSwapping = false;
		}
	}
}


// �޼���: ServerEquipButtonPressed_Implementation
// ����: �������� ����Ǵ� ��� ��ư ���� �Լ��Դϴ�.
void ALetsShootCharacter::ServerEquipButtonPressed_Implementation()
{
	if (Combat)
	{
		if (OverlappingWeapon)
		{
			// ��ġ�� ���Ⱑ ���� ��� ���⸦ �����մϴ�.
			Combat->EquipWeapon(OverlappingWeapon);
		}
		else if (Combat->ShouldSwapWeapons())
		{
			// ���⸦ ��ü�ؾ� �ϴ� ��� ���⸦ ��ü�մϴ�.
			Combat->SwapWeapons();
		}
	}
}

// �޼���: CrouchButtonPressed
// ����: �ɱ� ��ư�� ������ �� ȣ��Ǵ� �Լ��Դϴ�.
void ALetsShootCharacter::CrouchButtonPressed()
{
	if (Combat && Combat->bHoldingTheFlag) return;
	if (bDisableGameplay) return;
	if (bIsCrouched)
	{
		// �̹� �ɾ� �ִ� ��� ������ �Ͼ�� �մϴ�.
		UnCrouch();
	}
	else
	{
		// ���ִ� ��� ������ �ɰ� �մϴ�.
		Crouch();
	}
}

// �޼���: ReloadButtonPressed
// ����: ������ ��ư�� ������ �� ȣ��Ǵ� �Լ��Դϴ�.
void ALetsShootCharacter::ReloadButtonPressed()
{
	if (Combat && Combat->bHoldingTheFlag) return;
	if (bDisableGameplay) return;
	if (Combat)
	{
		// ���� ��ü�� ������ �Լ��� ȣ���մϴ�.
		Combat->Reload();
	}
}

// �޼���: AimButtonPressed
// ����: ���� ��ư�� ������ �� ȣ��Ǵ� �Լ��Դϴ�.
void ALetsShootCharacter::AimButtonPressed()
{
	if (Combat && Combat->bHoldingTheFlag) return;
	if (bDisableGameplay) return;
	if (Combat)
	{
		// ���� ��ü�� ���� ������ Ȱ��ȭ�մϴ�.
		Combat->SetAiming(true);
	}
}

// �޼���: AimButtonReleased
// ����: ���� ��ư�� ������ ���� �� ȣ��Ǵ� �Լ��Դϴ�.
void ALetsShootCharacter::AimButtonReleased()
{
	if (Combat && Combat->bHoldingTheFlag) return;
	if (bDisableGameplay) return;
	if (Combat)
	{
		// ���� ��ü�� ���� ������ ��Ȱ��ȭ�մϴ�.
		Combat->SetAiming(false);
	}
}

// �޼���: CalculateSpeed
// ����: �̵� �ӵ��� ����ϴ� �Լ��Դϴ�.
float ALetsShootCharacter::CalculateSpeed()
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;

	// �̵� �ӵ� ���
	return Velocity.Size();
}


// �޼���: AimOffset
// ����: ���� �������� ó���ϴ� �Լ��Դϴ�.
void ALetsShootCharacter::AimOffset(float DeltaTime)
{
	if (Combat && Combat->EquippedWeapon == nullptr) return;
	float Speed = CalculateSpeed();
	bool bIsInAir = GetCharacterMovement()->IsFalling();

	// �����ְ� ���� ���� �ƴ� ���
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

	// �޸��ų� ���߿� �ִ� ���
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

// �޼���: CalculateAO_Pitch
// ����: AO_Pitch ���� ����ϴ� �Լ��Դϴ�.
void ALetsShootCharacter::CalculateAO_Pitch()
{
	AO_Pitch = GetBaseAimRotation().Pitch;

	// AO_Pitch�� 90���� ũ�� ���� ��Ʈ���� �ƴ� ���
	if (AO_Pitch > 90.f && !IsLocallyControlled())
	{
		// pitch�� [270, 360) �������� [-90, 0) ������ �����մϴ�.
		FVector2D InRange(270.f, 360.f);
		FVector2D OutRange(-90.f, 0.f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

// �޼���: SimProxiesTurn
// ����: ���Ͻ� ȸ���� ó���ϴ� �Լ��Դϴ�.
void ALetsShootCharacter::SimProxiesTurn()
{
	if (Combat == nullptr || Combat->EquippedWeapon == nullptr) return;
	bRotateRootBone = false;
	float Speed = CalculateSpeed();

	// �̵� �ӵ��� 0���� ū ���
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
		// ���Ͻ� Yaw ������ �Ӱ谪���� ū ���
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


// �޼���: Jump
// ����: ���� ����� ó���ϴ� �Լ��Դϴ�.
void ALetsShootCharacter::Jump()
{
	if (Combat && Combat->bHoldingTheFlag) return;
	if (bDisableGameplay) return;
	if (bIsCrouched)
	{
		// �ɾ� �ִ� ���¿��� �����ϸ� �Ͼ�� �մϴ�.
		UnCrouch();
	}
	else
	{
		// �Ϲ����� ���� ������ �����մϴ�.
		Super::Jump();
	}
}

// �޼���: FireButtonPressed
// ����: �߻� ��ư�� ������ �� ȣ��Ǵ� �Լ��Դϴ�.
void ALetsShootCharacter::FireButtonPressed()
{
	if (Combat && Combat->bHoldingTheFlag) return;
	if (bDisableGameplay) return;
	if (Combat)
	{
		// ���� ��ü�� �߻� ��ư�� �������� �����մϴ�.
		Combat->FireButtonPressed(true);
	}
}

// �޼���: FireButtonReleased
// ����: �߻� ��ư�� ������ ���� �� ȣ��Ǵ� �Լ��Դϴ�.
void ALetsShootCharacter::FireButtonReleased()
{
	if (Combat && Combat->bHoldingTheFlag) return;
	if (bDisableGameplay) return;
	if (Combat)
	{
		// ���� ��ü�� �߻� ��ư�� �������� �����մϴ�.
		Combat->FireButtonPressed(false);
	}
}

// �޼���: TurnInPlace
// ����: ���ڸ����� ȸ���ϴ� ������ ó���ϴ� �Լ��Դϴ�.
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

// �޼���: HideCameraIfCharacterClose
// ����: ĳ���Ϳ� ī�޶��� �Ÿ��� ���� ī�޶�� ������ ���ü��� �����ϴ� �Լ��Դϴ�.
void ALetsShootCharacter::HideCameraIfCharacterClose()
{
	if (!IsLocallyControlled()) return;
	if ((FollowCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
	{
		// ĳ���Ϳ� ī�޶��� �Ÿ��� �Ӱ谪���� ���� ���
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
		// ĳ���Ϳ� ī�޶��� �Ÿ��� �Ӱ谪���� ū ���
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



// �޼���: OnRep_Health
// ����: Health ���� Replicate�Ǿ��� �� ȣ��Ǵ� �Լ��Դϴ�.
void ALetsShootCharacter::OnRep_Health(float LastHealth)
{
	UpdateHUDHealth();
	if (Health < LastHealth)
	{
		// Health�� ������ ��� �ǰ� ���� Montage�� ����մϴ�.
		PlayHitReactMontage();
	}
}

// �޼���: OnRep_Shield
// ����: Shield ���� Replicate�Ǿ��� �� ȣ��Ǵ� �Լ��Դϴ�.
void ALetsShootCharacter::OnRep_Shield(float LastShield)
{
	UpdateHUDShield();
	if (Shield < LastShield)
	{
		// Shield�� ������ ��� �ǰ� ���� Montage�� ����մϴ�.
		PlayHitReactMontage();
	}
}

// �޼���: UpdateHUDHealth
// ����: HUD�� Health ������ ������Ʈ�ϴ� �Լ��Դϴ�.
void ALetsShootCharacter::UpdateHUDHealth()
{
	LetsShootPlayerController = LetsShootPlayerController == nullptr ? Cast<ALetsShootPlayerController>(Controller) : LetsShootPlayerController;
	if (LetsShootPlayerController)
	{
		LetsShootPlayerController->SetHUDHealth(Health, MaxHealth);
	}
}

// �޼���: UpdateHUDShield
// ����: HUD�� Shield ������ ������Ʈ�ϴ� �Լ��Դϴ�.
void ALetsShootCharacter::UpdateHUDShield()
{
	LetsShootPlayerController = LetsShootPlayerController == nullptr ? Cast<ALetsShootPlayerController>(Controller) : LetsShootPlayerController;
	if (LetsShootPlayerController)
	{
		LetsShootPlayerController->SetHUDShield(Shield, MaxShield);
	}
}

// �޼���: UpdateHUDAmmo
// ����: HUD�� ź�� ������ ������Ʈ�ϴ� �Լ��Դϴ�.
void ALetsShootCharacter::UpdateHUDAmmo()
{
	LetsShootPlayerController = LetsShootPlayerController == nullptr ? Cast<ALetsShootPlayerController>(Controller) : LetsShootPlayerController;
	if (LetsShootPlayerController && Combat && Combat->EquippedWeapon)
	{
		LetsShootPlayerController->SetHUDCarriedAmmo(Combat->CarriedAmmo);
		LetsShootPlayerController->SetHUDWeaponAmmo(Combat->EquippedWeapon->GetAmmo());
	}
}

// �޼���: SpawnDefaultWeapon
// ����: �⺻ ���⸦ �����ϴ� �Լ��Դϴ�.
void ALetsShootCharacter::SpawnDefaultWeapon()
{
	LetsShootGameMode = LetsShootGameMode == nullptr ? GetWorld()->GetAuthGameMode<ALetsShootGameMode>() : LetsShootGameMode;
	UWorld* World = GetWorld();
	if (LetsShootGameMode && World && !bElimmed && DefaultWeaponClass)
	{
		// �⺻ ���⸦ �����Ͽ� Equip�մϴ�.
		AWeapon* StartingWeapon = World->SpawnActor<AWeapon>(DefaultWeaponClass);
		StartingWeapon->bDestroyWeapon = true;
		if (Combat)
		{
			Combat->EquipWeapon(StartingWeapon);
		}
	}
}



// �޼���: PollInit
// ����: �ʱ�ȭ �۾��� �����ϴ� �Լ��Դϴ�.
void ALetsShootCharacter::PollInit()
{
	if (LetsShootPlayerState == nullptr)
	{
		// LetsShootPlayerState�� nullptr�� ��� PlayerState�� �����ͼ� �ʱ�ȭ�մϴ�.
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

// �޼���: UpdateDissolveMaterial
// ����: Dissolve ������ ������Ʈ�ϴ� �Լ��Դϴ�.
void ALetsShootCharacter::UpdateDissolveMaterial(float DissolveValue)
{
	if (DynamicDissolveMaterialInstance)
	{
		DynamicDissolveMaterialInstance->SetScalarParameterValue(TEXT("Dissolve"), DissolveValue);
	}
}

// �޼���: StartDissolve
// ����: Dissolve ȿ���� �����ϴ� �Լ��Դϴ�.
void ALetsShootCharacter::StartDissolve()
{
	DissolveTrack.BindDynamic(this, &ALetsShootCharacter::UpdateDissolveMaterial);
	if (DissolveCurve && DissolveTimeline)
	{
		DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
		DissolveTimeline->Play();
	}
}

// �޼���: SetOverlappingWeapon
// ����: OverlappingWeapon�� �����ϴ� �Լ��Դϴ�.
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

// �޼���: OnRep_OverlappingWeapon
// ����: OverlappingWeapon�� Replicate�Ǿ��� �� ȣ��Ǵ� �Լ��Դϴ�.
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



// �޼���: IsWeaponEquipped
// ����: ���Ⱑ �����Ǿ� �ִ��� ���θ� ��ȯ�ϴ� �Լ��Դϴ�.
bool ALetsShootCharacter::IsWeaponEquipped()
{
	return (Combat && Combat->EquippedWeapon);
}

// �޼���: IsAiming
// ����: ���� ������ ���θ� ��ȯ�ϴ� �Լ��Դϴ�.
bool ALetsShootCharacter::IsAiming()
{
	return (Combat && Combat->bAiming);
}

// �޼���: GetEquippedWeapon
// ����: ���� ������ ���⸦ ��ȯ�ϴ� �Լ��Դϴ�.
AWeapon* ALetsShootCharacter::GetEquippedWeapon()
{
	if (Combat == nullptr) return nullptr;
	return Combat->EquippedWeapon;
}

// �޼���: GetHitTarget
// ����: ���� ����� ��ġ�� ��ȯ�ϴ� �Լ��Դϴ�.
FVector ALetsShootCharacter::GetHitTarget() const
{
	if (Combat == nullptr) return FVector();
	return Combat->HitTarget;
}

// �޼���: GetCombatState
// ����: ���� ���¸� ��ȯ�ϴ� �Լ��Դϴ�.
ECombatState ALetsShootCharacter::GetCombatState() const
{
	if (Combat == nullptr) return ECombatState::ECS_MAX;
	return Combat->CombatState;
}

// �޼���: IsLocallyReloading
// ����: ���ÿ��� ������ ������ ���θ� ��ȯ�ϴ� �Լ��Դϴ�.
bool ALetsShootCharacter::IsLocallyReloading()
{
	if (Combat == nullptr) return false;
	return Combat->bLocallyReloading;
}

// �޼���: IsHoldingTheFlag
// ����: ����� ��� �ִ��� ���θ� ��ȯ�ϴ� �Լ��Դϴ�.
bool ALetsShootCharacter::IsHoldingTheFlag() const
{
	if (Combat == nullptr) return false;
	return Combat->bHoldingTheFlag;
}

// �޼���: GetTeam
// ����: �÷��̾��� �Ҽ� ���� ��ȯ�ϴ� �Լ��Դϴ�.
ETeam ALetsShootCharacter::GetTeam()
{
	LetsShootPlayerState = LetsShootPlayerState == nullptr ? GetPlayerState<ALetsShootPlayerState>() : LetsShootPlayerState;
	if (LetsShootPlayerState == nullptr) return ETeam::ET_NoTeam;
	return LetsShootPlayerState->GetTeam();
}

// �޼���: SetHoldingTheFlag
// ����: ����� ��� �ִ��� ���θ� �����ϴ� �Լ��Դϴ�.
void ALetsShootCharacter::SetHoldingTheFlag(bool bHolding)
{
	if (Combat == nullptr) return;
	Combat->bHoldingTheFlag = bHolding;
}

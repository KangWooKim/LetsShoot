// Fill out your copyright notice in the Description page of Project Settings.


#include "Flag.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "LetsShoot/Character/LetsShootCharacter.h"

// 플래그 클래스의 생성자
AFlag::AFlag()
{
    // 플래그의 메쉬 컴포넌트를 생성하고 초기화합니다.
    FlagMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlagMesh"));

    // 플래그 메쉬를 루트 컴포넌트로 설정합니다.
    SetRootComponent(FlagMesh);

    // 영역 구와 픽업 위젯을 플래그 메쉬에 부착합니다.
    GetAreaSphere()->SetupAttachment(FlagMesh);
    GetPickupWidget()->SetupAttachment(FlagMesh);

    // 모든 채널에 대한 플래그 메쉬의 충돌 응답을 무시로 설정합니다.
    FlagMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

    // 플래그 메쉬의 충돌 기능을 비활성화합니다.
    FlagMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// 플래그가 드롭될 때의 동작을 정의하는 메서드
void AFlag::Dropped()
{
    // 플래그의 상태를 드롭된 상태로 설정합니다.
    SetWeaponState(EWeaponState::EWS_Dropped);

    // 플래그 메쉬를 부착된 컴포넌트에서 분리합니다.
    FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
    FlagMesh->DetachFromComponent(DetachRules);

    // 소유자를 null로 설정합니다.
    SetOwner(nullptr);
    LetsShootOwnerCharacter = nullptr;
    LetsShootOwnerController = nullptr;
}

// 플래그를 리셋하는 메서드
void AFlag::ResetFlag()
{
    // 현재 플래그의 소유자를 가져옵니다.
    ALetsShootCharacter* FlagBearer = Cast<ALetsShootCharacter>(GetOwner());

    if (FlagBearer)
    {
        // 플래그를 소유하고 있지 않음으로 설정합니다.
        FlagBearer->SetHoldingTheFlag(false);
        FlagBearer->SetOverlappingWeapon(nullptr);

        // 캐릭터의 웅크림 상태를 해제합니다.
        FlagBearer->UnCrouch();
    }

    // 서버 권한이 없는 경우 함수를 종료합니다.
    if (!HasAuthority()) return;

    FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
    FlagMesh->DetachFromComponent(DetachRules);
    SetWeaponState(EWeaponState::EWS_Initial);
    GetAreaSphere()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    GetAreaSphere()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECCollisionResponse::ECR_Overlap);

    SetOwner(nullptr);
    LetsShootOwnerCharacter = nullptr;
    LetsShootOwnerController = nullptr;

    // 플래그의 위치와 방향을 초기 상태로 설정합니다.
    SetActorTransform(InitialTransform);
}

// 플래그가 장착되었을 때의 동작을 정의하는 메서드
void AFlag::OnEquipped()
{
    // 픽업 위젯을 숨깁니다.
    ShowPickupWidget(false);

    // 영역 구의 충돌 기능을 비활성화합니다.
    GetAreaSphere()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 플래그 메쉬의 물리 시뮬레이션과 중력을 비활성화합니다.
    FlagMesh->SetSimulatePhysics(false);
    FlagMesh->SetEnableGravity(false);

    // 플래그 메쉬의 충돌 기능을 쿼리 전용으로 설정하고 월드 다이나믹 채널에 대한 충돌 응답을 오버랩으로 설정합니다.
    FlagMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    FlagMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECCollisionResponse::ECR_Overlap);

    // 커스텀 깊이를 비활성화합니다.
    EnableCustomDepth(false);
}


// 플래그가 드롭되었을 때의 동작을 정의하는 메서드
void AFlag::OnDropped()
{
    // 서버 권한이 있는 경우, 영역 구의 충돌 기능을 쿼리 전용으로 설정합니다.
    if (HasAuthority())
    {
        GetAreaSphere()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    }

    // 플래그 메쉬의 물리 시뮬레이션과 중력을 활성화합니다.
    FlagMesh->SetSimulatePhysics(true);
    FlagMesh->SetEnableGravity(true);

    // 플래그 메쉬의 충돌 기능을 쿼리 및 물리로 설정하고, 모든 채널에 대한 충돌 응답을 차단으로 설정합니다.
    FlagMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    FlagMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

    // 플래그 메쉬의 플레이어 캐릭터와 카메라 채널에 대한 충돌 응답을 무시로 설정합니다.
    FlagMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECCollisionResponse::ECR_Ignore);
    FlagMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECCollisionResponse::ECR_Ignore);

    // 플래그 메쉬의 사용자 정의 깊이 스텐실 값을 파란색으로 설정하고 렌더 상태를 업데이트합니다.
    FlagMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_BLUE);
    FlagMesh->MarkRenderStateDirty();

    // 사용자 정의 깊이를 활성화합니다.
    EnableCustomDepth(true);
}

// 게임이 시작되었을 때의 동작을 정의하는 메서드
void AFlag::BeginPlay()
{
    // 상위 클래스의 BeginPlay 메서드를 호출합니다.
    Super::BeginPlay();

    // 플래그의 초기 변환 값을 가져옵니다.
    InitialTransform = GetActorTransform();
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileGrenade.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// AProjectileGrenade 클래스 생성자
AProjectileGrenade::AProjectileGrenade()
{
    // 그레네이드 메시 컴포넌트 생성
    ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Grenade Mesh"));

    // 루트 컴포넌트에 그레네이드 메시를 첨부
    ProjectileMesh->SetupAttachment(RootComponent);

    // 그레네이드 메시의 충돌을 비활성화
    ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // 프로젝트 이동 컴포넌트 생성
    ProjectileMovementComponent = CreateDefaultSubobject<UPrjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

    // 이동 컴포넌트의 회전이 속도를 따르도록 설정
    ProjectileMovementComponent->bRotationFollowsVelocity = true;

    // 이동 컴포넌트 복제 허용
    ProjectileMovementComponent->SetIsReplicated(true);

    // 이동 컴포넌트의 바운스 허용
    ProjectileMovementComponent->bShouldBounce = true;
}

// BeginPlay 함수 오버라이드
void AProjectileGrenade::BeginPlay()
{
    // AActor 클래스의 BeginPlay 호출
    AActor::BeginPlay();

    // 트레일 시스템 생성
    SpawnTrailSystem();

    // 파괴 타이머 시작
    StartDestroyTimer();

    // 이동 컴포넌트의 바운스 이벤트에 함수 연결
    ProjectileMovementComponent->OnProjectileBounce.AddDynamic(this, &AProjectileGrenade::OnBounce);
}

// 그레네이드의 바운스 이벤트 핸들러
void AProjectileGrenade::OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
    // 바운스 사운드가 있을 경우 재생
    if (BounceSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            BounceSound,
            GetActorLocation()
        );
    }
}

// 그레네이드 파괴시 호출되는 함수
void AProjectileGrenade::Destroyed()
{
    // 폭발 데미지 적용
    ExplodeDamage();

    // AActor 클래스의 Destroyed 호출
    Super::Destroyed();
}

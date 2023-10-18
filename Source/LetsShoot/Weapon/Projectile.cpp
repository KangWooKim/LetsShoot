// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "LetsShoot/Character/LetsShootCharacter.h"
#include "LetsShoot/LetsShoot.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

// AProjectile 클래스 생성자입니다.
AProjectile::AProjectile()
{
    // 이 액터가 tick을 받을 수 있는지 설정합니다.
    PrimaryActorTick.bCanEverTick = true;

    // 네트워크 상에서 복제할 수 있는지 설정합니다.
    bReplicates = true;

    // CollisionBox 컴포넌트를 생성하고 root로 설정합니다.
    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    SetRootComponent(CollisionBox);

    // 충돌 설정입니다.
    CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECCollisionResponse::ECR_Block);
    CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECCollisionResponse::ECR_Block);
    CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECCollisionResponse::ECR_Block);
}

// BeginPlay 메서드입니다.
void AProjectile::BeginPlay()
{
    // 부모의 BeginPlay를 호출합니다.
    Super::BeginPlay();

    // Tracer가 있는 경우 TracerComponent를 붙입니다.
    if (Tracer)
    {
        TracerComponent = UGameplayStatics::SpawnEmitterAttached(
            Tracer,
            CollisionBox,
            FName(),
            GetActorLocation(),
            GetActorRotation(),
            EAttachLocation::KeepWorldPosition
        );
    }

    // 권한이 있는 경우 OnHit 이벤트를 등록합니다.
    if (HasAuthority())
    {
        CollisionBox->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);
    }
}

// OnHit 메서드입니다. 무언가에 부딪혔을 때 호출됩니다.
void AProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    // 이 액터를 파괴합니다.
    Destroy();
}

// SpawnTrailSystem 메서드입니다. 궤적 시스템을 생성합니다.
void AProjectile::SpawnTrailSystem()
{
    if (TrailSystem)
    {
        TrailSystemComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
            TrailSystem,
            GetRootComponent(),
            FName(),
            GetActorLocation(),
            GetActorRotation(),
            EAttachLocation::KeepWorldPosition,
            false
        );
    }
}


// AProjectile::ExplodeDamage 메서드입니다. 폭발 피해를 적용합니다.
void AProjectile::ExplodeDamage()
{
    // 발사한 Pawn을 가져옵니다.
    APawn* FiringPawn = GetInstigator();
    // 서버 권한이 있고, 발사한 Pawn이 있는 경우
    if (FiringPawn && HasAuthority())
    {
        // 발사한 Pawn의 컨트롤러를 가져옵니다.
        AController* FiringController = FiringPawn->GetController();
        if (FiringController)
        {
            // 원형 범위 내에 피해를 적용합니다. 
            UGameplayStatics::ApplyRadialDamageWithFalloff(
                this, // 월드 컨텍스트 오브젝트
                Damage, // 기본 피해
                10.f, // 최소 피해
                GetActorLocation(), // 중심 위치
                DamageInnerRadius, // 내부 반지름
                DamageOuterRadius, // 외부 반지름
                1.f, // 피해 감소율
                UDamageType::StaticClass(), // 피해 타입 클래스
                TArray<AActor*>(), // 무시할 액터들
                this, // 피해를 입힌 주체
                FiringController // 발사한 컨트롤러
            );
        }
    }
}

// Tick 메서드입니다. 
void AProjectile::Tick(float DeltaTime)
{
    // 부모의 Tick 메서드를 호출합니다.
    Super::Tick(DeltaTime);
}

// StartDestroyTimer 메서드입니다. 파괴 타이머를 시작합니다.
void AProjectile::StartDestroyTimer()
{
    // 타이머 매니저를 통해 타이머를 설정합니다.
    GetWorldTimerManager().SetTimer(
        DestroyTimer,
        this,
        &AProjectile::DestroyTimerFinished,
        DestroyTime
    );
}

// DestroyTimerFinished 메서드입니다. 파괴 타이머가 끝났을 때 호출됩니다.
void AProjectile::DestroyTimerFinished()
{
    // 이 액터를 파괴합니다.
    Destroy();
}

// Destroyed 메서드입니다. 액터가 파괴되었을 때 호출됩니다.
void AProjectile::Destroyed()
{
    // 부모의 Destroyed 메서드를 호출합니다.
    Super::Destroyed();

    // 충돌 파티클이 있는 경우, 파티클을 생성합니다.
    if (ImpactParticles)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, GetActorTransform());
    }
    // 충돌 사운드가 있는 경우, 사운드를 재생합니다.
    if (ImpactSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
    }
}


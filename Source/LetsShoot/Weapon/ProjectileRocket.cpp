// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileRocket.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "Sound/SoundCue.h"
#include "Components/BoxComponent.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "RocketMovementComponent.h"

// AProjectileRocket 클래스의 생성자를 정의
AProjectileRocket::AProjectileRocket()
{
    // "Rocket Mesh"라는 이름으로 UStaticMeshComponent를 생성
    // 이 메쉬는 프로젝타일의 모양을 결정
    ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rocket Mesh"));

    // 생성한 메쉬를 이 클래스의 RootComponent에 연결
    ProjectileMesh->SetupAttachment(RootComponent);

    // 메쉬의 충돌을 비활성화
    ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // "RocketMovementComponent"라는 이름으로 URocketMovementComponent를 생성
    // 이 컴포넌트는 로켓의 이동을 관리
    RocketMovementComponent = CreateDefaultSubobject<URocketMovementComponent>(TEXT("RocketMovementComponent"));

    // 로켓의 회전이 속도를 따르도록 설정
    RocketMovementComponent->bRotationFollowsVelocity = true;

    // 이 컴포넌트가 네트워크에서 복제되도록 설정
    RocketMovementComponent->SetIsReplicated(true);
}

// BeginPlay 메서드를 정의
// 이 메서드는 액터가 게임에서 활성화될 때 호출
void AProjectileRocket::BeginPlay()
{
    // 부모 클래스의 BeginPlay 호출
    Super::BeginPlay();

    // 이 액터가 서버에서만 실행되는 로직이 아니라면
    if (!HasAuthority())
    {
        // CollisionBox가 다른 컴포넌트와 충돌할 때 OnHit 함수가 호출되도록 설정
        CollisionBox->OnComponentHit.AddDynamic(this, &AProjectileRocket::OnHit);
    }

    // 이펙트 시스템 생성 함수 호출
    SpawnTrailSystem();

    // ProjectileLoop와 LoopingSoundAttenuation이 설정되어 있다면
    if (ProjectileLoop && LoopingSoundAttenuation)
    {
        // Rocket에 LoopingSound를 추가
        ProjectileLoopComponent = UGameplayStatics::SpawnSoundAttached(
            ProjectileLoop,
            GetRootComponent(),
            FName(),
            GetActorLocation(),
            EAttachLocation::KeepWorldPosition,
            false,
            1.0f,
            1.0f,
            0.0f,
            LoopingSoundAttenuation,
            (USoundConcurrency*)nullptr,
            false
        );
    }
}


// AProjectileRocket 클래스의 OnHit 메서드를 정의
// 이 메서드는 이 오브젝트가 다른 액터와 충돌했을 때 호출
void AProjectileRocket::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    // 만약 충돌한 액터가 이 프로젝타일의 소유자라면 아무 작업도 하지 않고 반환
    if (OtherActor == GetOwner())
    {
        return;
    }

    // 충돌로 인한 폭발 데미지를 처리
    ExplodeDamage();

    // 파괴 타이머 시작
    StartDestroyTimer();

    // ImpactParticles가 설정되어 있다면
    // 충돌 위치에 입자 시스템을 생성
    if (ImpactParticles)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, GetActorTransform());
    }

    // ImpactSound가 설정되어 있다면
    // 충돌 위치에서 사운드를 재생
    if (ImpactSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
    }

    // 프로젝타일 메쉬가 있다면
    // 이를 보이지 않게 설정
    if (ProjectileMesh)
    {
        ProjectileMesh->SetVisibility(false);
    }

    // 충돌 상자가 있다면
    // 이를 비활성화
    if (CollisionBox)
    {
        CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    // TrailSystemComponent와 그의 인스턴스가 있다면
    // 이를 비활성화
    if (TrailSystemComponent && TrailSystemComponent->GetSystemInstance())
    {
        TrailSystemComponent->GetSystemInstance()->Deactivate();
    }

    // ProjectileLoopComponent가 재생 중이라면
    // 이를 중지
    if (ProjectileLoopComponent && ProjectileLoopComponent->IsPlaying())
    {
        ProjectileLoopComponent->Stop();
    }
}

// AProjectileRocket 클래스의 Destroyed 메서드를 정의
// 이 메서드는 이 오브젝트가 파괴될 때 호출
void AProjectileRocket::Destroyed()
{

}

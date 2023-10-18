// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"
#include "Kismet/GameplayStatics.h"
#include "LetsShoot/Character/LetsShootCharacter.h"
#include "LetsShoot/PlayerController/LetsShootPlayerController.h"
#include "LetsShoot/LetsShootComponents/DelayProcessingComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// 프로젝트 탄환 클래스 생성자
AProjectileBullet::AProjectileBullet()
{
    //프로젝트 이동 컴포넌트 생성
    ProjectileMovementComponent = CreateDefaultSubobject<UPrjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

    //이동 컴포넌트의 회전이 속도를 따르게 설정
    ProjectileMovementComponent->bRotationFollowsVelocity = true;

    // 이동 컴포넌트 복제 허용
    ProjectileMovementComponent->SetIsReplicated(true);

    // 이동 컴포넌트의 초기 속도 및 최대 속도 설정
    ProjectileMovementComponent->InitialSpeed ​​ = InitialSpeed;
    ProjectileMovementComponent->MaxSpeed ​​ = InitialSpeed;
}

// 에디터에서 속성 변경시 호출되는 함수
#if WITH_EDITOR
void AProjectileBullet::PostEditChangeProperty(FPropertyChangedEvent& Event)
{
    Super::PostEditChangeProperty(Event);

    // 변경된 속성의 이름 가져오기
    FName PropertyName = Event.Property != nullptr ? Event.Property->GetFName() : NAME_None;

    // 초기 속도 속성이 변경된 경우 이동 컴포넌트의 초기 및 최대 속도 업데이트
    if (PropertyName == GET_MEMBER_NAME_CHECKED(AProjectileBullet, InitialSpeed))
    {
        if (ProjectileMovementComponent)
        {
            ProjectileMovementComponent->InitialSpeed ​​ = InitialSpeed;
            ProjectileMovementComponent->MaxSpeed ​​ = InitialSpeed;
        }
    }
}
#endif

// 탄환이 대상에 맞았을 때 호출되는 함수
void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    // 발사체의 소유자 정보를 가져오기
    ALetsShootCharacter* OwnerCharacter = Cast<ALetsShootCharacter>(GetOwner());

    // 소유자 정보가 유효한 경우
    if (OwnerCharacter)
    {
        // 소유자의 컨트롤러 정보 가져오기
        ALetsShootPlayerController* OwnerController = Cast<ALetsShootPlayerController>(OwnerCharacter->Controller);

        if (OwnerController)
        {
            // 서버 리와인드 기능이 비활성화되고 소유자에게 권한이 있는 경우
            if (OwnerCharacter->HasAuthority() && !bUseServerSideRewind)
            {
                // 헤드샷인 경우 헤드샷 대미지를, 아닌 경우 기본 대미지를 적용
                const float DamageToCause = Hit.BoneName.ToString() == FString("head") ? HeadShotDamage : Damage;

                // 대미지 적용
                UGameplayStatics::ApplyDamage(OtherActor, DamageToCause, OwnerController, this, UDamageType::StaticClass());
                Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
                return;
            }

            // 맞은 캐릭터 정보를 가져오기
            ALetsShootCharacter* HitCharacter = Cast<ALetsShootCharacter>(OtherActor);

            // 서버 리와인드 기능이 활성화되고 소유자가 로컬에서 조작 중이며, 맞은 캐릭터가 유효한 경우
            if (bUseServerSideRewind && OwnerCharacter->GetLagCompensation() && OwnerCharacter->IsLocallyControlled() && HitCharacter)
            {
                // 서버에 스코어 요청
                OwnerCharacter->GetLagCompensation()->ProjectileServerScoreRequest(
                    HitCharacter,
                    TraceStart,
                    InitialVelocity,
                    OwnerController->GetServerTime() - OwnerController->SingleTripTime
                );
            }
        }
    }

    Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}



void AProjectileBullet::BeginPlay()
{
	Super::BeginPlay();
	/*
	FPredictProjectilePathParams PathParams;
	PathParams.bTraceWithChannel = true;
	PathParams.bTraceWithCollision = true;
	PathParams.DrawDebugTime = 5.f;
	PathParams.DrawDebugType = EDrawDebugTrace::ForDuration;
	PathParams.LaunchVelocity = GetActorForwardVector() * InitialSpeed;
	PathParams.MaxSimTime = 4.f;
	PathParams.ProjectileRadius = 5.f;
	PathParams.SimFrequency = 30.f;
	PathParams.StartLocation = GetActorLocation();
	PathParams.TraceChannel = ECollisionChannel::ECC_Visibility;
	PathParams.ActorsToIgnore.Add(this);

	FPredictProjectilePathResult PathResult;

	UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult);*/
}

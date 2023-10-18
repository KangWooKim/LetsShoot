// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "LetsShoot/Character/LetsShootCharacter.h"
#include "LetsShoot/PlayerController/LetsShootPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "WeaponTypes.h"
#include "LetsShoot/LetsShootComponents/LagCompensationComponent.h"

// 탄환을 발사하는 함수
void AHitScanWeapon::Fire(const FVector& HitTarget)
{
    // 부모 클래스의 Fire 함수를 호출합니다.
    Super::Fire(HitTarget);

    // 소유자 Pawn을 가져옵니다.
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    // 소유자 Pawn이 없으면 함수를 종료합니다.
    if (OwnerPawn == nullptr) return;

    // 소유자 Pawn의 컨트롤러를 가져옵니다.
    AController* InstigatorController = OwnerPawn->GetController();

    // 무기 메쉬에서 "MuzzleFlash" 소켓을 가져옵니다.
    const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
    if (MuzzleFlashSocket)
    {
        // 소켓의 변환 값을 가져옵니다.
        FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
        FVector Start = SocketTransform.GetLocation();

        FHitResult FireHit;
        // 무기 발사 히트 결과를 가져옵니다.
        WeaponTraceHit(Start, HitTarget, FireHit);

        // 히트 결과로부터 블래스터 캐릭터를 가져옵니다.
        ALetsShootCharacter* LetsShootCharacter = Cast<ALetsShootCharacter>(FireHit.GetActor());
        if (LetsShootCharacter && InstigatorController)
        {
            // 데미지를 입히는 조건을 확인합니다.
            bool bCauseAuthDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
            if (HasAuthority() && bCauseAuthDamage)
            {
                // 머리에 맞았는지 여부에 따라 데미지 값을 결정합니다.
                const float DamageToCause = FireHit.BoneName.ToString() == FString("head") ? HeadShotDamage : Damage;

                // 데미지를 적용합니다.
                UGameplayStatics::ApplyDamage(
                    LetsShootCharacter,
                    DamageToCause,
                    InstigatorController,
                    this,
                    UDamageType::StaticClass()
                );
            }
            // 서버 사이드 리와인드를 사용하는 경우
            if (!HasAuthority() && bUseServerSideRewind)
            {
                LetsShootOwnerCharacter = LetsShootOwnerCharacter == nullptr ? Cast<ALetsShootCharacter>(OwnerPawn) : LetsShootOwnerCharacter;
                LetsShootOwnerController = LetsShootOwnerController == nullptr ? Cast<ALetsShootPlayerController>(InstigatorController) : LetsShootOwnerController;
                if (LetsShootOwnerController && LetsShootOwnerCharacter && LetsShootOwnerCharacter->GetLagCompensation() && LetsShootOwnerCharacter->IsLocallyControlled())
                {
                    // 점수 요청을 서버에 전달합니다.
                    LetsShootOwnerCharacter->GetLagCompensation()->ServerScoreRequest(
                        LetsShootCharacter,
                        Start,
                        HitTarget,
                        LetsShootOwnerController->GetServerTime() - LetsShootOwnerController->SingleTripTime
                    );
                }
            }
        }
        // 파티클 이펙트를 생성합니다.
        if (ImpactParticles)
        {
            UGameplayStatics::SpawnEmitterAtLocation(
                GetWorld(),
                ImpactParticles,
                FireHit.ImpactPoint,
                FireHit.ImpactNormal.Rotation()
            );
        }
        // 히트 사운드를 재생합니다.
        if (HitSound)
        {
            UGameplayStatics::PlaySoundAtLocation(
                this,
                HitSound,
                FireHit.ImpactPoint
            );
        }

        // 머즐 플래시 이펙트를 생성합니다.
        if (MuzzleFlash)
        {
            UGameplayStatics::SpawnEmitterAtLocation(
                GetWorld(),
                MuzzleFlash,
                SocketTransform
            );
        }
        // 발사 사운드를 재생합니다.
        if (FireSound)
        {
            UGameplayStatics::PlaySoundAtLocation(
                this,
                FireSound,
                GetActorLocation()
            );
        }
    }
}

// 무기에서 히트 결과를 추적하는 함수
void AHitScanWeapon::WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit)
{
    // 현재 월드를 가져옵니다.
    UWorld* World = GetWorld();
    if (World)
    {
        // 히트 타겟 방향으로 추적 끝점을 계산합니다.
        FVector End = TraceStart + (HitTarget - TraceStart) * 1.25f;

        // 선 추적을 수행하여 히트 결과를 가져옵니다.
        World->LineTraceSingleByChannel(
            OutHit,
            TraceStart,
            End,
            ECollisionChannel::ECC_Visibility
        );
        FVector BeamEnd = End;
        // 블로킹 히트가 발생하면 빔 끝점을 히트 위치로 설정합니다.
        if (OutHit.bBlockingHit)
        {
            BeamEnd = OutHit.ImpactPoint;
        }
        // 블로킹 히트가 없으면 히트 위치를 추적 끝점으로 설정합니다.
        else
        {
            OutHit.ImpactPoint = End;
        }

        // 빔 파티클을 생성합니다.
        if (BeamParticles)
        {
            UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
                World,
                BeamParticles,
                TraceStart,
                FRotator::ZeroRotator,
                true
            );
            // 생성한 빔 파티클의 타겟 위치를 설정합니다.
            if (Beam)
            {
                Beam->SetVectorParameter(FName("Target"), BeamEnd);
            }
        }
    }
}

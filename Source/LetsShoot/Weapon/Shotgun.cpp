// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "LetsShoot/Character/LetsShootCharacter.h"
#include "LetsShoot/PlayerController/LetsShootPlayerController.h"
#include "LetsShoot/LetsShootComponents/DelayProcessingComponent.h"
#include "Kismet/GameplayStatics.h"
#include "particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"

// AShotgun 클래스의 FireShotgun 메서드 정의

void AShotgun::FireShotgun(const TArray<FVector_NetQuantize>& HitTargets)
{
    // 부모 클래스의 Fire 메서드 호출
    AWeapon::Fire(FVector());
    // 소유자 Pawn을 가져옴
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    // 소유자 Pawn이 없는 경우, 메서드 종료
    if (OwnerPawn == nullptr) return;
    // 인스티게이터 컨트롤러 가져옴
    AController* InstigatorController = OwnerPawn->GetController();

    const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
    if (MuzzleFlashSocket)
    {
        const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
        const FVector Start = SocketTransform.GetLocation();

        // 적 캐릭터에 대한 히트 횟수를 매핑
        TMap<ALetsShootCharacter*, uint32> HitMap;
        TMap<ALetsShootCharacter*, uint32> HeadShotHitMap;
        for (FVector_NetQuantize HitTarget : HitTargets)
        {
            FHitResult FireHit;
            WeaponTraceHit(Start, HitTarget, FireHit);

            ALetsShootCharacter* LetsShootCharacter = Cast<ALetsShootCharacter>(FireHit.GetActor());
            if (LetsShootCharacter)
            {
                const bool bHeadShot = FireHit.BoneName.ToString() == FString("head");

                // 헤드샷인 경우
                if (bHeadShot)
                {
                    if (HeadShotHitMap.Contains(LetsShootCharacter)) HeadShotHitMap[LetsShootCharacter]++;
                    else HeadShotHitMap.Emplace(LetsShootCharacter, 1);
                }
                // 몸통 샷인 경우
                else
                {
                    if (HitMap.Contains(LetsShootCharacter)) HitMap[LetsShootCharacter]++;
                    else HitMap.Emplace(LetsShootCharacter, 1);
                }

                // 충돌 시 이펙트 생성
                if (ImpactParticles)
                {
                    UGameplayStatics::SpawnEmitterAtLocation(
                        GetWorld(),
                        ImpactParticles,
                        FireHit.ImpactPoint,
                        FireHit.ImpactNormal.Rotation()
                    );
                }
                // 충돌 시 사운드 재생
                if (HitSound)
                {
                    UGameplayStatics::PlaySoundAtLocation(
                        this,
                        Hit Sound,
                        FireHit.ImpactPoint,
                        .5f,
                        FMath::FRandRange(-.5f, .5f)
                    );
                }
            }
        }

        TArray<ALetsShootCharacter*> HitCharacters;
        TMap<ALetsShootCharacter*, float> DamageMap;

        // 히트 맵을 순회하며 몸통 샷에 의한 총 데미지 계산
        for (auto HitPair : HitMap)
        {
            if (HitPair.Key)
            {
                DamageMap.Emplace(HitPair.Key, HitPair.Value * Damage);
                HitCharacters.AddUnique(HitPair.Key);
            }
        }

        // 헤드샷 히트 맵을 순회하며 헤드샷에 의한 총 데미지 계산
        for (auto HeadShotHitPair : HeadShotHitMap)
        {
            if (HeadShotHitPair.Key)
            {
                if (DamageMap.Contains(HeadShotHitPair.Key)) DamageMap[HeadShotHitPair.Key] += HeadShotHitPair.Value * HeadShotDamage;
                else DamageMap.Emplace(HeadShotHitPair.Key, HeadShotHitPair.Value * HeadShotDamage);

                HitCharacters.AddUnique(HeadShotHitPair.Key);
            }
        }

        // 데미지 맵을 순회하며 각 캐릭터에 대한 총 데미지 적용
        for (auto DamagePair : DamageMap)
        {
            if (DamagePair.Key && InstigatorController)
            {
                bool bCauseAuthDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
                if (HasAuthority() && bCauseAuthDamage)
                {
                    UGameplayStatics::ApplyDamage(
                        DamagePair.Key, // 피해를 입은 캐릭터
                        DamagePair.Value, // 위의 두 for 루프에서 계산된 데미지
                        InstigatorController,
                        this,
                        UdamageType::StaticClass()
                    );
                }
            }
        }

        // 서버 측 리와인드 사용 여부에 따른 처리
        if (!HasAuthority() && bUseServerSideRewind)
        {
            LetsShootOwnerCharacter = LetsShootOwnerCharacter == nullptr ? Cast<ALetsShootCharacter>(OwnerPawn) : LetsShootOwnerCharacter;
            LetsShootOwnerController = LetsShootOwnerController == nullptr ? Cast<ALetsShootPlayerController>(InstigatorController) : LetsShootOwnerController;
            if (LetsShootOwnerController && LetsShootOwnerCharacter && LetsShootOwnerCharacter->GetLagCompensation() && LetsShootOwnerCharacter->IsLocallyControlled())
            {
                LetsShootOwnerCharacter->GetLagCompensation()->ShotgunServerScoreRequest(
                    Hit Characters,
                    start,
                    HitTargets,
                    LetsShootOwnerController->GetServerTime() - LetsShootOwnerController->SingleTripTime
                );
            }
        }
    }
}


// AShotgun 클래스의 ShotgunTraceEndWithScatter 메서드 정의
// HitTarget: 샷건이 목표로 하는 위치입니다.
// HitTargets: 샷건에서 발사된 펠릿들이 목표로하는 위치들을 담은 배열입니다. 이 메서드에서는 이 배열에 펠릿들이 목표로 하는 위치들을 추가합니다.
void AShotgun::ShotgunTraceEndWithScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets)
{
    // 무기 메시에서 "MuzzleFlash" 소켓을 가져옴
    const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
    // MuzzleFlashSocket이 없는 경우, 메서드 종료
    if (MuzzleFlashSocket == nullptr) return;

    // 소켓의 변환(위치, 회전, 크기)을 가져옴
    const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
    // 트레이스 시작 위치를 소켓의 위치로 설정
    const FVector TraceStart = SocketTransform.GetLocation();

    // 목표 지점으로의 단위 벡터 계산
    const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
    // 구의 중심을 계산
    const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;

    // 펠릿의 갯수만큼 루프를 돌며 히트 목표를 계산
    for (uint32 i = 0; i < NumberOfPellets; i++)
    {
        // 구의 반지름 안에서 무작위 위치를 생성
        const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
        // 끝 위치를 계산
        const FVector EndLoc = SphereCenter + RandVec;
        // 시작 위치에서 끝 위치까지의 벡터 계산
        FVector ToEndLoc = EndLoc - TraceStart;
        // 트레이스 길이를 고려하여 실제 끝 위치 계산
        ToEndLoc = TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size();

        // 계산된 끝 위치를 히트 목표에 추가
        HitTargets.Add(ToEndLoc);
    }
}


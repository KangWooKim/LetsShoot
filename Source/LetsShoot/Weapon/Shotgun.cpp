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

// AShotgun Ŭ������ FireShotgun �޼��� ����

void AShotgun::FireShotgun(const TArray<FVector_NetQuantize>& HitTargets)
{
    // �θ� Ŭ������ Fire �޼��� ȣ��
    AWeapon::Fire(FVector());
    // ������ Pawn�� ������
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    // ������ Pawn�� ���� ���, �޼��� ����
    if (OwnerPawn == nullptr) return;
    // �ν�Ƽ������ ��Ʈ�ѷ� ������
    AController* InstigatorController = OwnerPawn->GetController();

    const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
    if (MuzzleFlashSocket)
    {
        const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
        const FVector Start = SocketTransform.GetLocation();

        // �� ĳ���Ϳ� ���� ��Ʈ Ƚ���� ����
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

                // ��弦�� ���
                if (bHeadShot)
                {
                    if (HeadShotHitMap.Contains(LetsShootCharacter)) HeadShotHitMap[LetsShootCharacter]++;
                    else HeadShotHitMap.Emplace(LetsShootCharacter, 1);
                }
                // ���� ���� ���
                else
                {
                    if (HitMap.Contains(LetsShootCharacter)) HitMap[LetsShootCharacter]++;
                    else HitMap.Emplace(LetsShootCharacter, 1);
                }

                // �浹 �� ����Ʈ ����
                if (ImpactParticles)
                {
                    UGameplayStatics::SpawnEmitterAtLocation(
                        GetWorld(),
                        ImpactParticles,
                        FireHit.ImpactPoint,
                        FireHit.ImpactNormal.Rotation()
                    );
                }
                // �浹 �� ���� ���
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

        // ��Ʈ ���� ��ȸ�ϸ� ���� ���� ���� �� ������ ���
        for (auto HitPair : HitMap)
        {
            if (HitPair.Key)
            {
                DamageMap.Emplace(HitPair.Key, HitPair.Value * Damage);
                HitCharacters.AddUnique(HitPair.Key);
            }
        }

        // ��弦 ��Ʈ ���� ��ȸ�ϸ� ��弦�� ���� �� ������ ���
        for (auto HeadShotHitPair : HeadShotHitMap)
        {
            if (HeadShotHitPair.Key)
            {
                if (DamageMap.Contains(HeadShotHitPair.Key)) DamageMap[HeadShotHitPair.Key] += HeadShotHitPair.Value * HeadShotDamage;
                else DamageMap.Emplace(HeadShotHitPair.Key, HeadShotHitPair.Value * HeadShotDamage);

                HitCharacters.AddUnique(HeadShotHitPair.Key);
            }
        }

        // ������ ���� ��ȸ�ϸ� �� ĳ���Ϳ� ���� �� ������ ����
        for (auto DamagePair : DamageMap)
        {
            if (DamagePair.Key && InstigatorController)
            {
                bool bCauseAuthDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
                if (HasAuthority() && bCauseAuthDamage)
                {
                    UGameplayStatics::ApplyDamage(
                        DamagePair.Key, // ���ظ� ���� ĳ����
                        DamagePair.Value, // ���� �� for �������� ���� ������
                        InstigatorController,
                        this,
                        UdamageType::StaticClass()
                    );
                }
            }
        }

        // ���� �� �����ε� ��� ���ο� ���� ó��
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


// AShotgun Ŭ������ ShotgunTraceEndWithScatter �޼��� ����
// HitTarget: ������ ��ǥ�� �ϴ� ��ġ�Դϴ�.
// HitTargets: ���ǿ��� �߻�� �縴���� ��ǥ���ϴ� ��ġ���� ���� �迭�Դϴ�. �� �޼��忡���� �� �迭�� �縴���� ��ǥ�� �ϴ� ��ġ���� �߰��մϴ�.
void AShotgun::ShotgunTraceEndWithScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets)
{
    // ���� �޽ÿ��� "MuzzleFlash" ������ ������
    const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
    // MuzzleFlashSocket�� ���� ���, �޼��� ����
    if (MuzzleFlashSocket == nullptr) return;

    // ������ ��ȯ(��ġ, ȸ��, ũ��)�� ������
    const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
    // Ʈ���̽� ���� ��ġ�� ������ ��ġ�� ����
    const FVector TraceStart = SocketTransform.GetLocation();

    // ��ǥ ���������� ���� ���� ���
    const FVector ToTargetNormalized = (HitTarget - TraceStart).GetSafeNormal();
    // ���� �߽��� ���
    const FVector SphereCenter = TraceStart + ToTargetNormalized * DistanceToSphere;

    // �縴�� ������ŭ ������ ���� ��Ʈ ��ǥ�� ���
    for (uint32 i = 0; i < NumberOfPellets; i++)
    {
        // ���� ������ �ȿ��� ������ ��ġ�� ����
        const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
        // �� ��ġ�� ���
        const FVector EndLoc = SphereCenter + RandVec;
        // ���� ��ġ���� �� ��ġ������ ���� ���
        FVector ToEndLoc = EndLoc - TraceStart;
        // Ʈ���̽� ���̸� �����Ͽ� ���� �� ��ġ ���
        ToEndLoc = TraceStart + ToEndLoc * TRACE_LENGTH / ToEndLoc.Size();

        // ���� �� ��ġ�� ��Ʈ ��ǥ�� �߰�
        HitTargets.Add(ToEndLoc);
    }
}

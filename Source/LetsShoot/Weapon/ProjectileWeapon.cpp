// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectile.h"

// AProjectileWeapon 클래스의 Fire 메서드를 정의
// 이 메서드는 무기 발사를 처리
void AProjectileWeapon::Fire(const FVector& HitTarget)
{
    // 부모 클래스의 Fire 메서드를 호출
    Super::Fire(HitTarget);

    // 소유자를 APawn 타입으로 캐스팅
    APawn* InstigatorPawn = Cast<APawn>(GetOwner());

    // 무기의 머즐 플래시 소켓을 가져옴
    const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));

    // 현재 월드를 가져옴
    UWorld* World = GetWorld();

    // 머즐 플래시 소켓과 월드가 모두 유효한 경우에만 실행
    if (MuzzleFlashSocket && World)
    {
        // 머즐 플래시 소켓의 위치를 구함
        FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());

        // 머즐 플래시 소켓 위치에서 적중 위치까지의 벡터를 구함
        FVector ToTarget = HitTarget - SocketTransform.GetLocation();

        // 적중 벡터의 방향으로 회전 벡터를 구함
        FRotator TargetRotation = ToTarget.Rotation();

        // 액터를 스폰하기 위한 파라미터를 설정
        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = GetOwner();
        SpawnParams.Instigator = InstigatorPawn;

        // 스폰될 프로젝타일 변수 선언
        AProjectile* SpawnedProjectile = nullptr;

        // 서버 사이드 리와인드(SSR) 사용 여부에 따른 분기
        // SSR이란, 클라이언트와 서버 간의 지연시간을 보상하기 위해 총알 발사를 서버에서 처리하는 기능
        if (bUseServerSideRewind)
        {
            // 서버에서의 처리
            if (InstigatorPawn->HasAuthority()) // 서버에서 동작
            {
                if (InstigatorPawn->IsLocallyControlled()) // 서버, 호스트 - 복제된 프로젝타일 사용
                {
                    SpawnedProjectile = World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
                    SpawnedProjectile->bUseServerSideRewind = false;
                    SpawnedProjectile->Damage = Damage;
                    SpawnedProjectile->HeadShotDamage = HeadShotDamage;
                }
                else // 서버, 로컬 제어되지 않음 - 비 복제 프로젝타일 스폰, SSR 사용
                {
                    SpawnedProjectile = World->SpawnActor<AProjectile>(ServerSideRewindProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
                    SpawnedProjectile->bUseServerSideRewind = true;
                }
            }
            else // 클라이언트에서 SSR 사용
            {
                if (InstigatorPawn->IsLocallyControlled()) // 클라이언트, 로컬 제어 - 비 복제 프로젝타일 스폰, SSR 사용
                {
                    SpawnedProjectile = World->SpawnActor<AProjectile>(ServerSideRewindProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
                    SpawnedProjectile->bUseServerSideRewind = true;
                    SpawnedProjectile->TraceStart = SocketTransform.GetLocation();
                    SpawnedProjectile->InitialVelocity = SpawnedProjectile->GetActorForwardVector() * SpawnedProjectile->InitialSpeed;
                }
                else // 클라이언트, 로컬 제어되지 않음 - 비 복제 프로젝타일 스폰, SSR 사용 안 함
                {
                    SpawnedProjectile = World->SpawnActor<AProjectile>(ServerSideRewindProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
                    SpawnedProjectile->bUseServerSideRewind = false;
                }
            }
        }
        else // SSR을 사용하지 않는 무기
        {
            if (InstigatorPawn->HasAuthority())
            {
                SpawnedProjectile = World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
                SpawnedProjectile->bUseServerSideRewind = false;
                SpawnedProjectile->Damage = Damage;
                SpawnedProjectile->HeadShotDamage = HeadShotDamage;
            }
        }
    }
}

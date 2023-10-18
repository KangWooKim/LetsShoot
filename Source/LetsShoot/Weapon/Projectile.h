// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

// AProjectile 클래스 선언입니다. 이 클래스는 발사체를 나타냅니다.
UCLASS()
class LETSSHOOT_API AProjectile : public AActor
{
	GENERATED_BODY()

public:
	// 생성자입니다.
	AProjectile();

	// Tick 메서드 오버라이드입니다. 각 프레임마다 호출됩니다.
	// DeltaTime: 이전 프레임과의 시간 차이
	virtual void Tick(float DeltaTime) override;

	// Destroyed 메서드 오버라이드입니다. 액터가 파괴되었을 때 호출됩니다.
	virtual void Destroyed() override;

	// 서버 측 리와인드를 사용하는지 여부
	bool bUseServerSideRewind = false;

	// 트레이스 시작 위치
	FVector_NetQuantize TraceStart;

	// 초기 속도
	FVector_NetQuantize100 InitialVelocity;

	// 발사체의 초기 속도 (기본값: 15000)
	UPROPERTY(EditAnywhere)
		float InitialSpeed = 15000;

	// 피해량 (기본값: 20.f). 이는 그레네이드와 로켓에만 적용됩니다.
	UPROPERTY(EditAnywhere)
		float Damage = 20.f;

	// 헤드샷 피해량 (기본값: 40.f). 이는 그레네이드와 로켓에는 적용되지 않습니다.
	UPROPERTY(EditAnywhere)
		float HeadShotDamage = 40.f;

protected:
	// BeginPlay 메서드 오버라이드입니다. 액터가 게임에 배치되면 호출됩니다.
	virtual void BeginPlay() override;

	// 파괴 타이머를 시작하는 메서드
	void StartDestroyTimer();

	// 파괴 타이머가 끝났을 때 호출되는 메서드
	void DestroyTimerFinished();

	// 꼬리 파티클 시스템을 생성하는 메서드
	void SpawnTrailSystem();

	// 폭발 피해를 적용하는 메서드
	void ExplodeDamage();

	// 컴포넌트가 다른 액터나 컴포넌트와 충돌했을 때 호출되는 메서드
	UFUNCTION()
		virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// 충돌 시 생성되는 파티클
	UPROPERTY(EditAnywhere)
		class UParticleSystem* ImpactParticles;

	// 충돌 시 재생되는 사운드
	UPROPERTY(EditAnywhere)
		class USoundCue* ImpactSound;

	// 발사체의 충돌 박스
	UPROPERTY(EditAnywhere)
		class UBoxComponent* CollisionBox;

	// 꼬리 파티클 시스템
	UPROPERTY(EditAnywhere)
		class UNiagaraSystem* TrailSystem;

	// 꼬리 파티클 시스템 컴포넌트
	UPROPERTY()
		class UNiagaraComponent* TrailSystemComponent;

	// 발사체의 움직임을 관리하는 컴포넌트
	UPROPERTY(VisibleAnywhere)
		class UProjectileMovementComponent* ProjectileMovementComponent;

	// 발사체의 메쉬
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* ProjectileMesh;

	// 피해를 입히는 범위의 내부 반지름 (기본값: 200.f)
	UPROPERTY(EditAnywhere)
		float DamageInnerRadius = 200.f;

	// 피해를 입히는 범위의 외부 반지름 (기본값: 500.f)
	UPROPERTY(EditAnywhere)
		float DamageOuterRadius = 500.f;

private:
	// 트레이서 파티클 시스템입니다. 발사체의 궤적을 시각적으로 나타내는 역할을 합니다.
	UPROPERTY(EditAnywhere)
		UParticleSystem* Tracer;

	// 트레이서 파티클 시스템 컴포넌트입니다. 이를 통해 트레이서 파티클 시스템의 위치, 회전, 스케일 등을 제어할 수 있습니다.
	UPROPERTY()
		class UParticleSystemComponent* TracerComponent;

	// 발사체의 파괴를 위한 타이머입니다. 설정된 시간이 지나면 발사체는 파괴됩니다.
	FTimerHandle DestroyTimer;

	// 발사체가 파괴되기까지의 시간입니다. 단위는 초입니다. 기본값은 3초입니다.
	UPROPERTY(EditAnywhere)
		float DestroyTime = 3.f;

public:	
};

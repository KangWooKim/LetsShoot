// Fill out your copyright notice in the Description page of Project Settings.


#include "Casing.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
// 탄피 클래스 생성자
ACasing::ACasing()
{
	// 이 Actor는 tick이 필요 없으므로 false로 설정합니다.
	PrimaryActorTick.bCanEverTick = false;

	// 탄피의 3D 메쉬 컴포넌트를 생성합니다.
	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	// 이 메쉬를 RootComponent로 설정합니다.
	SetRootComponent(CasingMesh);
	// 카메라와의 충돌 응답을 무시하도록 설정합니다.
	CasingMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECCollisionResponse::ECR_Ignore);
	// 물리 시뮬레이션을 활성화합니다.
	CasingMesh->SetSimulatePhysics(true);
	// 중력을 활성화합니다.
	CasingMesh->SetEnableGravity(true);
	// 강체 충돌 알림을 활성화합니다.
	CasingMesh->SetNotifyRigidBodyCollision(true);
	// 탄피가 총에서 나올 때의 힘을 설정합니다.
	ShellEjectionImpulse = 10.f;
}



void ACasing::BeginPlay()
{
	// 부모 클래스의 BeginPlay를 호출합니다.
	Super::BeginPlay();

	// 탄피 메쉬가 충돌 시 OnHit 함수를 호출하도록 설정합니다.
	CasingMesh->OnComponentHit.AddDynamic(this, &ACasing::OnHit);
	// 총에서 탄피가 나오도록 추진력을 부여합니다.
	CasingMesh->AddImpulse(GetActorForwardVector() * ShellEjectionImpulse);
}

// 충돌 시 호출되는 함수
void ACasing::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 탄피가 충돌했을 때 사운드가 설정되어 있다면 사운드를 재생합니다.
	if (ShellSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());
	}
	// 탄피 Actor를 파괴합니다.
	Destroy();
}


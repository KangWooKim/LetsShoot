// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/SphereComponent.h"
#include "LetsShoot/Weapon/WeaponTypes.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

// AItem 클래스의 생성자입니다.
AItem::AItem()
{
    // 이 객체가 매 틱마다 업데이트 될 수 있는지 여부를 설정합니다.
    PrimaryActorTick.bCanEverTick = true;

    // 이 객체가 네트워크를 통해 복제되는지 여부를 설정합니다.
    bReplicates = true;

    // 이 객체의 루트 컴포넌트를 설정합니다.
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    // 이 객체에 스피어 컴포넌트를 추가하고 설정합니다.
    OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
    OverlapSphere->SetupAttachment(RootComponent);  // 루트 컴포넌트에 붙입니다.
    OverlapSphere->SetSphereRadius(150.f);  // 스피어의 반지름을 설정합니다.
    // 스피어의 충돌 설정을 업데이트합니다.
    OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    OverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECCollisionResponse::ECR_Overlap);
    OverlapSphere->AddLocalOffset(FVector(0.f, 0.f, 85.f));  // 스피어의 위치를 조정합니다.

    // 이 객체에 메시 컴포넌트를 추가하고 설정합니다.
    ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
    ItemMesh->SetupAttachment(OverlapSphere);  // 스피어 컴포넌트에 붙입니다.
    // 메시의 충돌 설정을 업데이트합니다.
    ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ItemMesh->SetRelativeScale3D(FVector(5.f, 5.f, 5.f));  // 메시의 크기를 설정합니다.
    ItemMesh->SetRenderCustomDepth(true);  // 메시의 커스텀 뎁스 렌더링을 활성화합니다.
    ItemMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_PURPLE);  // 메시의 커스텀 뎁스 스텐실 값을 설정합니다.

    // 이 객체에 니아가라 컴포넌트를 추가하고 설정합니다.
    ItemEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ItemEffectComponent"));
    ItemEffectComponent->SetupAttachment(RootComponent);  // 루트 컴포넌트에 붙입니다.
}

// 이 메서드는 게임이 시작할 때 호출됩니다.
void AItem::BeginPlay()
{
    // 부모 클래스의 BeginPlay 메서드를 호출합니다.
    Super::BeginPlay();

    // 이 코드가 권한을 가진 서버에서만 실행되도록 합니다.
    if (HasAuthority())
    {
        // 월드의 타이머 매니저를 가져와서 타이머를 설정합니다.
        GetWorldTimerManager().SetTimer(
            BindOverlapTimer,  // 이 타이머를 설정합니다.
            this,  // 타이머가 끝날 때 호출되는 메서드의 객체를 설정합니다.
            &AItem::BindOverlapTimerFinished,  // 타이머가 끝날 때 호출되는 메서드를 설정합니다.
            BindOverlapTime  // 타이머의 지속 시간을 설정합니다.
        );
    }
}


// OverlapSphere와 다른 컴포넌트가 겹치기 시작할 때 호출되는 메서드입니다.
void AItem::OnSphereOverlap(
    UPrimitiveComponent* OverlappedComponent,  // 겹치기 시작한 컴포넌트입니다.
    AActor* OtherActor,  // 겹치기 시작한 다른 액터입니다.
    UPrimitiveComponent* OtherComp,  // 겹치기 시작한 다른 컴포넌트입니다.
    int32 OtherBodyIndex,  // 겹친 다른 컴포넌트의 바디 인덱스입니다.
    bool bFromSweep,  // 이 겹침이 스윕으로부터 발생했는지의 여부입니다.
    const FHitResult& SweepResult  // 스윕의 결과입니다.
) {

}

// OverlapSphere의 OnComponentBeginOverlap 이벤트를 바인딩하는 타이머가 끝났을 때 호출됩니다.
void AItem::BindOverlapTimerFinished() {
    OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
}

// 매 프레임마다 호출되는 메서드입니다.
void AItem::Tick(float DeltaTime) {  // DeltaTime: 이전 프레임과의 시간 차입니다.
    Super::Tick(DeltaTime);

    if (ItemMesh) {
        ItemMesh->AddWorldRotation(FRotator(0.f, BaseTurnRate * DeltaTime, 0.f));  // 메시를 회전시킵니다.
    }
}

// 이 액터가 파괴될 때 호출되는 메서드입니다.
void AItem::Destroyed() {
    Super::Destroyed();

    if (ItemSound) {
        // 액터의 위치에서 소리를 재생합니다.
        UGameplayStatics::PlaySoundAtLocation(
            this,
            ItemSound,
            GetActorLocation()
        );
    }
    if (ItemEffect) {
        // 액터의 위치와 회전에서 파티클 시스템을 스폰합니다.
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            this,
            ItemEffect,
            GetActorLocation(),
            GetActorRotation()
        );
    }
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/SphereComponent.h"
#include "LetsShoot/Weapon/WeaponTypes.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

// AItem Ŭ������ �������Դϴ�.
AItem::AItem()
{
    // �� ��ü�� �� ƽ���� ������Ʈ �� �� �ִ��� ���θ� �����մϴ�.
    PrimaryActorTick.bCanEverTick = true;

    // �� ��ü�� ��Ʈ��ũ�� ���� �����Ǵ��� ���θ� �����մϴ�.
    bReplicates = true;

    // �� ��ü�� ��Ʈ ������Ʈ�� �����մϴ�.
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    // �� ��ü�� ���Ǿ� ������Ʈ�� �߰��ϰ� �����մϴ�.
    OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
    OverlapSphere->SetupAttachment(RootComponent);  // ��Ʈ ������Ʈ�� ���Դϴ�.
    OverlapSphere->SetSphereRadius(150.f);  // ���Ǿ��� �������� �����մϴ�.
    // ���Ǿ��� �浹 ������ ������Ʈ�մϴ�.
    OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    OverlapSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    OverlapSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECCollisionResponse::ECR_Overlap);
    OverlapSphere->AddLocalOffset(FVector(0.f, 0.f, 85.f));  // ���Ǿ��� ��ġ�� �����մϴ�.

    // �� ��ü�� �޽� ������Ʈ�� �߰��ϰ� �����մϴ�.
    ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
    ItemMesh->SetupAttachment(OverlapSphere);  // ���Ǿ� ������Ʈ�� ���Դϴ�.
    // �޽��� �浹 ������ ������Ʈ�մϴ�.
    ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ItemMesh->SetRelativeScale3D(FVector(5.f, 5.f, 5.f));  // �޽��� ũ�⸦ �����մϴ�.
    ItemMesh->SetRenderCustomDepth(true);  // �޽��� Ŀ���� ���� �������� Ȱ��ȭ�մϴ�.
    ItemMesh->SetCustomDepthStencilValue(CUSTOM_DEPTH_PURPLE);  // �޽��� Ŀ���� ���� ���ٽ� ���� �����մϴ�.

    // �� ��ü�� �Ͼư��� ������Ʈ�� �߰��ϰ� �����մϴ�.
    ItemEffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ItemEffectComponent"));
    ItemEffectComponent->SetupAttachment(RootComponent);  // ��Ʈ ������Ʈ�� ���Դϴ�.
}

// �� �޼���� ������ ������ �� ȣ��˴ϴ�.
void AItem::BeginPlay()
{
    // �θ� Ŭ������ BeginPlay �޼��带 ȣ���մϴ�.
    Super::BeginPlay();

    // �� �ڵ尡 ������ ���� ���������� ����ǵ��� �մϴ�.
    if (HasAuthority())
    {
        // ������ Ÿ�̸� �Ŵ����� �����ͼ� Ÿ�̸Ӹ� �����մϴ�.
        GetWorldTimerManager().SetTimer(
            BindOverlapTimer,  // �� Ÿ�̸Ӹ� �����մϴ�.
            this,  // Ÿ�̸Ӱ� ���� �� ȣ��Ǵ� �޼����� ��ü�� �����մϴ�.
            &AItem::BindOverlapTimerFinished,  // Ÿ�̸Ӱ� ���� �� ȣ��Ǵ� �޼��带 �����մϴ�.
            BindOverlapTime  // Ÿ�̸��� ���� �ð��� �����մϴ�.
        );
    }
}


// OverlapSphere�� �ٸ� ������Ʈ�� ��ġ�� ������ �� ȣ��Ǵ� �޼����Դϴ�.
void AItem::OnSphereOverlap(
    UPrimitiveComponent* OverlappedComponent,  // ��ġ�� ������ ������Ʈ�Դϴ�.
    AActor* OtherActor,  // ��ġ�� ������ �ٸ� �����Դϴ�.
    UPrimitiveComponent* OtherComp,  // ��ġ�� ������ �ٸ� ������Ʈ�Դϴ�.
    int32 OtherBodyIndex,  // ��ģ �ٸ� ������Ʈ�� �ٵ� �ε����Դϴ�.
    bool bFromSweep,  // �� ��ħ�� �������κ��� �߻��ߴ����� �����Դϴ�.
    const FHitResult& SweepResult  // ������ ����Դϴ�.
) {

}

// OverlapSphere�� OnComponentBeginOverlap �̺�Ʈ�� ���ε��ϴ� Ÿ�̸Ӱ� ������ �� ȣ��˴ϴ�.
void AItem::BindOverlapTimerFinished() {
    OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
}

// �� �����Ӹ��� ȣ��Ǵ� �޼����Դϴ�.
void AItem::Tick(float DeltaTime) {  // DeltaTime: ���� �����Ӱ��� �ð� ���Դϴ�.
    Super::Tick(DeltaTime);

    if (ItemMesh) {
        ItemMesh->AddWorldRotation(FRotator(0.f, BaseTurnRate * DeltaTime, 0.f));  // �޽ø� ȸ����ŵ�ϴ�.
    }
}

// �� ���Ͱ� �ı��� �� ȣ��Ǵ� �޼����Դϴ�.
void AItem::Destroyed() {
    Super::Destroyed();

    if (ItemSound) {
        // ������ ��ġ���� �Ҹ��� ����մϴ�.
        UGameplayStatics::PlaySoundAtLocation(
            this,
            ItemSound,
            GetActorLocation()
        );
    }
    if (ItemEffect) {
        // ������ ��ġ�� ȸ������ ��ƼŬ �ý����� �����մϴ�.
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            this,
            ItemEffect,
            GetActorLocation(),
            GetActorRotation()
        );
    }
}

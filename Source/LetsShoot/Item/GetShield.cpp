// Fill out your copyright notice in the Description page of Project Settings.


#include "GetShield.h"
#include "LetsShoot/Character/LetsShootCharacter.h"
#include "LetsShoot/LetsShootComponents/BuffComponent.h"

// AGetShield Ŭ������ OnSphereOverlap �Լ�
void AGetShield::OnSphereOverlap(
    UPrimitiveComponent* OverlappedComponent,   // ������ �̺�Ʈ�� �߻��� ������Ʈ
    AActor* OtherActor,                         // ������ �̺�Ʈ�� �߻���Ų �ٸ� ����
    UPrimitiveComponent* OtherComp,             // ������ �̺�Ʈ�� �߻���Ų �ٸ� ������Ʈ
    int32 OtherBodyIndex,                       // �������� �߻��� �ٸ� ������Ʈ�� �ٵ� �ε���
    bool bFromSweep,                            // ���� ����κ����� ���������� ����
    const FHitResult& SweepResult               // ������ �߻����� ���� ��� ����
)
{
    // �θ� Ŭ������ ������ ó�� �Լ� ȣ��
    Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

    // �������� �߻��� ���Ͱ� ALetsShootCharacter �ν��Ͻ����� Ȯ��
    ALetsShootCharacter* LetsShootCharacter = Cast<ALetsShootCharacter>(OtherActor);
    if (LetsShootCharacter)
    {
        // ALetsShootCharacter �ν��Ͻ����� UBuffComponent�� ������
        UBuffComponent* Buff = LetsShootCharacter->GetBuff();
        if (Buff)
        {
            // UBuffComponent�� ���и� ����
            Buff->ReplenishShield(ShieldReplenishAmount, ShieldReplenishTime);
        }
    }

    // ������ ó���� ������ �ڽ��� �ı�
    Destroy();
}

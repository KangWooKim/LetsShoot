// Fill out your copyright notice in the Description page of Project Settings.


#include "GetSpeed.h"
#include "LetsShoot/Character/LetsShootCharacter.h"
#include "LetsShoot/LetsShootComponents/BuffComponent.h"

// ���ǵ� �Ⱦ��� �������� ���۵Ǿ��� �� ȣ��Ǵ� �޼���
void AGetSpeed::OnSphereOverlap(
    UPrimitiveComponent* OverlappedComponent, // �������� �⺻ ������Ʈ
    AActor* OtherActor, // �������� �ٸ� ����
    UPrimitiveComponent* OtherComp, // �������� �ٸ� ������Ʈ
    int32 OtherBodyIndex, // �������� �ٸ� ������Ʈ�� �ٵ� �ε���
    bool bFromSweep, // �������κ��� �������� ���۵Ǿ����� ����
    const FHitResult& SweepResult // ������ ���
) {
    // �θ� Ŭ������ OnSphereOverlap �޼��� ȣ��
    Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

    // OtherActor�� ALetsShootCharacter�� ĳ����
    ALetsShootCharacter* LetsShootCharacter = Cast<ALetsShootCharacter>(OtherActor);
    // ĳ���ÿ� �����ϸ�
    if (LetsShootCharacter)
    {
        // LetsShootCharacter�� BuffComponent�� ����
        UBuffComponent* Buff = LetsShootCharacter->GetBuff();
        // Buff�� ��ȿ�ϸ�
        if (Buff)
        {
            // Buff�� �ӵ��� ������Ŵ
            Buff->BuffSpeed(BaseSpeedBuff, CrouchSpeedBuff, SpeedBuffTime);
        }
    }

    // ���ǵ� �Ⱦ��� �ı�
    Destroy();
}

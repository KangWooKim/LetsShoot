// Fill out your copyright notice in the Description page of Project Settings.


#include "GetJump.h"
#include "LetsShoot/Character/LetsShootCharacter.h"
#include "LetsShoot/LetsShootComponents/BuffComponent.h"

void AGetJump::OnSphereOverlap(
    UPrimitiveComponent* OverlappedComponent,  // �� ��ü�� ��ġ�� ������Ʈ
    AActor* OtherActor,  // �� ��ü�� ��ġ�� �ٸ� ����
    UPrimitiveComponent* OtherComp,  // �� ��ü�� ��ġ�� �ٸ� ������Ʈ
    int32 OtherBodyIndex,  // ��ġ�� �ٸ� ������Ʈ�� ��ü �ε���
    bool bFromSweep,  // �� ��ħ�� ���� ���� �߻��ߴ��� ����
    const FHitResult& SweepResult  // ���� ���� �߻��� ù �浹�� ���� ���
) {
    // �θ� Ŭ������ OnSphereOverlap �޼��带 ȣ���մϴ�.
    Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

    // ��ġ�� �ٸ� ���͸� ALetsShootCharacter�� ��ȯ(ĳ����) �õ��մϴ�.
    ALetsShootCharacter* LetsShootCharacter = Cast<ALetsShootCharacter>(OtherActor);
    // �����ϸ� �Ʒ� �ڵ带 �����մϴ�.
    if (LetsShootCharacter)
    {
        // LetsShootCharacter�� Buff ������Ʈ�� ����ϴ�.
        UBuffComponent* Buff = LetsShootCharacter->GetBuff();
        // Buff ������Ʈ�� ������ �Ʒ� �ڵ带 �����մϴ�.
        if (Buff)
        {
            // Buff�� ���� �ɷ��� ��ȭ�մϴ�.
            Buff->BuffJump(JumpZVelocityBuff, JumpBuffTime);
        }
    }

    // �� ��ü�� �ı��մϴ�.
    Destroy();
}

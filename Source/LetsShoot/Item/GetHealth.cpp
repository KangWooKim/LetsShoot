// Fill out your copyright notice in the Description page of Project Settings.


#include "GetHealth.h"
#include "LetsShoot/Character/LetsShootCharacter.h"
#include "LetsShoot/LetsShootComponents/BuffComponent.h"

// AGetHealth Ŭ������ �������Դϴ�.
// �� Ŭ������ ��ü�� ��Ʈ��ũ���� ����(replicate) �����ϵ��� �����մϴ�.
AGetHealth::AGetHealth()
{
	bReplicates = true;
}

// �� ��ü�� �ٸ� ��ü�� ��ġ�� ȣ��Ǵ� �޼����Դϴ�.
void AGetHealth::OnSphereOverlap(
	UPrimitiveComponent* OverlappedComponent, // �� ��ü�� ��ģ ������Ʈ
	AActor* OtherActor, // �� ��ü�� ��ģ �ٸ� ����
	UPrimitiveComponent* OtherComp, // �� ��ü�� ��ģ �ٸ� ������Ʈ
	int32 OtherBodyIndex, // ��ģ �ٸ� ������Ʈ�� ��ü �ε���
	bool bFromSweep, // �� ��ħ�� �������κ��� �߻��� �������� ��Ÿ��
	const FHitResult& SweepResult // ���� ���� �߻��� ù ��° �浹�� ���� ���
)
{
	// �θ� Ŭ������ OnSphereOverlap �޼��带 ȣ���մϴ�.
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	// ��ģ �ٸ� ���Ͱ� ALetsShootCharacter���� Ȯ���մϴ�.
	ALetsShootCharacter* LetsShootCharacter = Cast<ALetsShootCharacter>(OtherActor);
	if (LetsShootCharacter)
	{
		// ��ģ ALetsShootCharacter�� BuffComponent�� �����ɴϴ�.
		UBuffComponent* Buff = LetsShootCharacter->GetBuff();
		if (Buff)
		{
			// BuffComponent�� Heal �޼��带 ȣ���Ͽ� �÷��̾��� ü���� ȸ����ŵ�ϴ�.
			Buff->Heal(HealAmount, HealingTime);
		}
	}

	// �� ��ü�� �ı��մϴ�.
	Destroy();
}

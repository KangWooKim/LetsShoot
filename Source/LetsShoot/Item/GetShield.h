// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "GetShield.generated.h"

/**
 * 
 */
UCLASS()
// LETSSHOOT_API�� ���� DLL �Ǵ� ���� ���̺귯���� ������ ���� ���
// AItem Ŭ������ ��ӹ޴� AGetShield Ŭ����
class LETSSHOOT_API AGetShield : public AItem
{
    // Unreal Engine�� Ŭ���� ���� ��ũ��
    GENERATED_BODY()

protected:
    // �������� �߻����� �� ȣ��Ǵ� �Լ�. AGetShield Ŭ���������� �� �Լ��� �������մϴ�.
    virtual void OnSphereOverlap(
        UPrimitiveComponent* OverlappedComponent,   // �������� ������Ʈ
        AActor* OtherActor,                         // �������� �ٸ� ����
        UPrimitiveComponent* OtherComp,             // �������� �ٸ� ������Ʈ
        int32 OtherBodyIndex,                       // �������� �ٵ��� �ε���
        bool bFromSweep,                            // �������� ���� �߻��ߴ��� ����
        const FHitResult& SweepResult               // ���� ���
    ) override;

private:
    // ���и� �����ϴ� ���� �����ϴ� ����. EditAnywhere�� ���� �����Ϳ��� ���� ����
    UPROPERTY(EditAnywhere)
    float ShieldReplenishAmount = 100.f;

    // ���и� �����ϴ� �� �ɸ��� �ð��� �����ϴ� ����. EditAnywhere�� ���� �����Ϳ��� ���� ����
    UPROPERTY(EditAnywhere)
    float ShieldReplenishTime = 5.f;
};


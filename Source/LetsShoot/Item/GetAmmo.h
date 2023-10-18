// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "LetsShoot/Weapon/WeaponTypes.h"
#include "GetAmmo.generated.h"

/**
 * 
 */
UCLASS()
// 'LETSSHOOT_API'�� ���� �������� �� Ŭ������ �������� ����ϵ��� �����մϴ�.
// 'AItem'�� �� Ŭ������ �θ� Ŭ�����Դϴ�.
class LETSSHOOT_API AGetAmmo : public AItem
{
	// 'GENERATED_BODY()'�� UE5�� �� Ŭ������ �ʿ��� �߰� ����� �ڵ����� �߰��ϵ��� �ϴ� ��ũ���Դϴ�.
	GENERATED_BODY()

protected:
	// OnSphereOverlap �޼���� �� ��ü�� �ٸ� ��ü�� ��ĥ �� ȣ��Ǵ� �޼����Դϴ�.
	// �� �޼���� �θ� Ŭ������ OnSphereOverlap �޼��带 ������(override)�մϴ�.
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent, // �� ��ü�� ��ģ ������Ʈ
		AActor* OtherActor, // �� ��ü�� ��ģ �ٸ� ����
		UPrimitiveComponent* OtherComp, // �� ��ü�� ��ģ �ٸ� ������Ʈ
		int32 OtherBodyIndex, // ��ģ �ٸ� ������Ʈ�� ��ü �ε���
		bool bFromSweep, // �� ��ħ�� �������κ��� �߻��� �������� ��Ÿ��
		const FHitResult& SweepResult // ���� ���� �߻��� ù ��° �浹�� ���� ���
	);

private:
	// �Ⱦ� ������ ź���� ���� ��Ÿ���� �����Դϴ�.
	// 'EditAnywhere'�� �� ������ UE5 �������� ��𼭵� ������ �� �ֵ��� �մϴ�.
	UPROPERTY(EditAnywhere)
		int32 AmmoAmount = 30;

	// ź���� ������ ��Ÿ���� �����Դϴ�.
	// 'EWeaponType'�� ������ ������ ��Ÿ���� �������Դϴ�.
	// 'EditAnywhere'�� �� ������ UE5 �������� ��𼭵� ������ �� �ֵ��� �մϴ�.
	UPROPERTY(EditAnywhere)
		EWeaponType WeaponType;
};

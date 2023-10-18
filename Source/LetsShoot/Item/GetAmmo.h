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
// 'LETSSHOOT_API'는 빌드 설정에서 이 클래스를 공용으로 사용하도록 설정합니다.
// 'AItem'은 이 클래스의 부모 클래스입니다.
class LETSSHOOT_API AGetAmmo : public AItem
{
	// 'GENERATED_BODY()'는 UE5가 이 클래스에 필요한 추가 기능을 자동으로 추가하도록 하는 매크로입니다.
	GENERATED_BODY()

protected:
	// OnSphereOverlap 메서드는 이 객체와 다른 객체가 겹칠 때 호출되는 메서드입니다.
	// 이 메서드는 부모 클래스의 OnSphereOverlap 메서드를 재정의(override)합니다.
	virtual void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent, // 이 객체와 겹친 컴포넌트
		AActor* OtherActor, // 이 객체와 겹친 다른 액터
		UPrimitiveComponent* OtherComp, // 이 객체와 겹친 다른 컴포넌트
		int32 OtherBodyIndex, // 겹친 다른 컴포넌트의 본체 인덱스
		bool bFromSweep, // 이 겹침이 스윕으로부터 발생한 것인지를 나타냄
		const FHitResult& SweepResult // 스윕 동안 발생한 첫 번째 충돌에 대한 결과
	);

private:
	// 픽업 가능한 탄약의 양을 나타내는 변수입니다.
	// 'EditAnywhere'는 이 변수를 UE5 에디터의 어디서든 수정할 수 있도록 합니다.
	UPROPERTY(EditAnywhere)
		int32 AmmoAmount = 30;

	// 탄약의 종류를 나타내는 변수입니다.
	// 'EWeaponType'는 무기의 종류를 나타내는 열거형입니다.
	// 'EditAnywhere'는 이 변수를 UE5 에디터의 어디서든 수정할 수 있도록 합니다.
	UPROPERTY(EditAnywhere)
		EWeaponType WeaponType;
};

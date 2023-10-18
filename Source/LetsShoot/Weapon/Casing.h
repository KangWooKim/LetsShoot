// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Casing.generated.h"

UCLASS()
// "ACasing"은 탄피를 나타내는 클래스입니다.
class LETSSHOOT_API ACasing : public AActor
{
	GENERATED_BODY()

public:
	// 기본 생성자입니다.
	ACasing();

protected:
	// 게임 시작 시 호출되는 함수입니다.
	virtual void BeginPlay() override;

	// 충돌 시 호출되는 함수입니다.
	// HitComp: 충돌이 발생한 이 컴포넌트입니다.
	// OtherActor: 이 컴포넌트와 충돌한 다른 액터입니다.
	// OtherComp: 충돌한 다른 액터의 컴포넌트입니다.
	// NormalImpulse: 충돌 시 주어진 충격입니다.
	// Hit: 충돌에 대한 상세 정보입니다.
	UFUNCTION()
		virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	// 탄피의 메쉬 컴포넌트입니다.
	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* CasingMesh;

	// 탄피가 총에서 나올 때의 추진력입니다.
	UPROPERTY(EditAnywhere)
		float ShellEjectionImpulse;

	// 탄피가 충돌 시 재생되는 사운드입니다.
	UPROPERTY(EditAnywhere)
		class USoundCue* ShellSound;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class LETSSHOOT_API AItem : public AActor
{
	GENERATED_BODY()

public:
	AItem(); // 생성자입니다.
	virtual void Tick(float DeltaTime) override; // 프레임마다 호출됩니다. DeltaTime은 이전 프레임과의 시간 차입니다.
	virtual void Destroyed() override; // 이 액터가 파괴될 때 호출됩니다.

protected:
	virtual void BeginPlay() override; // 게임이 시작되었을 때 호출됩니다.

	// 이 함수는 OverlapSphere와 다른 컴포넌트가 겹치기 시작할 때 호출됩니다.
	UFUNCTION()
		virtual void OnSphereOverlap(
			UPrimitiveComponent* OverlappedComponent, // 겹치기 시작한 컴포넌트입니다.
			AActor* OtherActor, // 겹치기 시작한 다른 액터입니다.
			UPrimitiveComponent* OtherComp, // 겹치기 시작한 다른 컴포넌트입니다.
			int32 OtherBodyIndex, // 겹친 다른 컴포넌트의 바디 인덱스입니다.
			bool bFromSweep, // 이 겹침이 스윕으로부터 발생했는지의 여부입니다.
			const FHitResult& SweepResult // 스윕의 결과입니다.
		);

	// 회전 속도를 조절합니다. 단위는 초당 도입니다.
	UPROPERTY(EditAnywhere)
		float BaseTurnRate = 45.f;

private:
	// 플레이어가 픽업을 수집할 수 있는 영역을 나타냅니다.
	UPROPERTY(EditAnywhere)
		class USphereComponent* OverlapSphere;

	// 픽업이 수집될 때 재생되는 사운드입니다.
	UPROPERTY(EditAnywhere)
		class USoundCue* ItemSound;

	// 픽업의 메시를 나타냅니다.
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* ItemMesh;

	// 픽업에 첨부된 효과를 나타냅니다.
	UPROPERTY(VisibleAnywhere)
		class UNiagaraComponent* ItemEffectComponent;

	// 픽업이 수집될 때 스폰되는 효과입니다.
	UPROPERTY(EditAnywhere)
		class UNiagaraSystem* ItemEffect;

	// OverlapSphere의 OnComponentBeginOverlap 이벤트를 바인딩하는 타이머를 제어합니다.
	FTimerHandle BindOverlapTimer;
	float BindOverlapTime = 0.25f;
	void BindOverlapTimerFinished(); // BindOverlapTimer가 끝났을 때 호출됩니다.

public:
};

// Fill out your copyright notice in the Description page of Project Settings.


#include "DelayProcessingComponent.h"
#include "LetsShoot/Character/LetsShootCharacter.h"
#include "LetsShoot/Weapon/Weapon.h"
#include "Components/BoxComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LetsShoot/LetsShoot.h"
#include "DrawDebugHelpers.h"

// Lag Compensation 구성 요소의 생성자
// 주요 구성 요소 틱이 가능하도록 설정
UDelayProcessingComponent::UDelayProcessingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// 게임 시작 시 호출되는 함수
void UDelayProcessingComponent::BeginPlay()
{
	Super::BeginPlay();  // 부모 클래스의 BeginPlay 함수 호출
}

// 두 프레임 간의 보간을 수행하는 함수
// 이 함수는 렉 보정을 위해 사용됨
FFramePackage UDelayProcessingComponent::InterpBetweenFrames(const FFramePackage& OlderFrame, const FFramePackage& YoungerFrame, float HitTime)
{
	// 두 프레임 간의 시간 거리 계산
	const float Distance = YoungerFrame.Time - OlderFrame.Time;

	// 보간 비율 계산
	// 클램프 함수를 사용하여 보간 비율이 0과 1 사이에 머무르도록 함
	const float InterpFraction = FMath::Clamp((HitTime - OlderFrame.Time) / Distance, 0.f, 1.f);

	// 보간 프레임 패키지 생성
	FFramePackage InterpFramePackage;
	InterpFramePackage.Time = HitTime; // 보간 프레임 패키지의 시간을 타격 시간으로 설정

	// 각 충돌 상자에 대해 보간
	for (auto& YoungerPair : YoungerFrame.HitBoxInfo)
	{
		const FName& BoxInfoName = YoungerPair.Key;

		const FBoxInformation& OlderBox = OlderFrame.HitBoxInfo[BoxInfoName];
		const FBoxInformation& YoungerBox = YoungerFrame.HitBoxInfo[BoxInfoName];

		FBoxInformation InterpBoxInfo;

		// 위치와 회전에 대해 보간 수행
		InterpBoxInfo.Location = FMath::VInterpTo(OlderBox.Location, YoungerBox.Location, 1.f, InterpFraction);
		InterpBoxInfo.Rotation = FMath::RInterpTo(OlderBox.Rotation, YoungerBox.Rotation, 1.f, InterpFraction);

		// 크기는 젊은 상자의 크기를 사용
		InterpBoxInfo.BoxExtent = YoungerBox.BoxExtent;

		// 결과 보간 상자를 패키지에 추가
		InterpFramePackage.HitBoxInfo.Add(BoxInfoName, InterpBoxInfo);
	}

	// 결과 보간 프레임 패키지 반환
	return InterpFramePackage;
}

// 공격이 명중했는지 확인하는 함수
// 패키지, 명중한 캐릭터, 트레이스 시작 위치, 명중 위치를 인수로 받음
FServerSideRewindResult UDelayProcessingComponent::ConfirmHit(const FFramePackage& Package, ALetsShootCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation)
{
	if (HitCharacter == nullptr) return FServerSideRewindResult(); // 명중한 캐릭터가 없으면 결과 반환

	// 현재 프레임 생성 및 상자 위치 캐시
	FFramePackage CurrentFrame;
	CacheBoxPositions(HitCharacter, CurrentFrame);

	// 상자 이동
	MoveBoxes(HitCharacter, Package);

	// 캐릭터 메시 충돌 비활성화
	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::NoCollision);

	// 머리 충돌을 먼저 활성화
	UBoxComponent* HeadBox = HitCharacter->HitCollisionBoxes[FName("head")];
	HeadBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	HeadBox->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);

	// 명중 확인을 위한 히트 결과 생성
	FHitResult ConfirmHitResult;

	// 트레이스 끝 지점 계산
	const FVector TraceEnd = TraceStart + (HitLocation - TraceStart) * 1.25f;

	UWorld* World = GetWorld();
	if (World)
	{
		// 라인 트레이스를 실행하여 명중 확인
		World->LineTraceSingleByChannel(
			ConfirmHitResult,
			TraceStart,
			TraceEnd,
			ECC_HitBox
		);
		if (ConfirmHitResult.bBlockingHit) // 머리를 맞췄다면 일찍 반환
		{
			ResetHitBoxes(HitCharacter, CurrentFrame);
			EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
			return FServerSideRewindResult{ true, true };
		}
		else // 머리를 못 맞췄다면 다른 상자들 확인
		{
			for (auto& HitBoxPair : HitCharacter->HitCollisionBoxes)
			{
				if (HitBoxPair.Value != nullptr)
				{
					HitBoxPair.Value->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
					HitBoxPair.Value->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);
				}
			}
			World->LineTraceSingleByChannel(
				ConfirmHitResult,
				TraceStart,
				TraceEnd,
				ECC_HitBox
			);
			if (ConfirmHitResult.bBlockingHit)
			{
				ResetHitBoxes(HitCharacter, CurrentFrame);
				EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
				return FServerSideRewindResult{ true, false };
			}
		}
	}

	// 충돌이 없었다면 상자들을 리셋하고 메시 충돌을 다시 활성화한 후 결과 반환
	ResetHitBoxes(HitCharacter, CurrentFrame);
	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
	return FServerSideRewindResult{ false, false };
}


// 렉 보상 요소에서 발사체의 명중을 확인하는 메서드
// 해당 메서드는 주어진 프레임 패키지와 캐릭터, 발사 위치, 초기 속도, 명중 시간을 이용하여 
// 발사체가 캐릭터를 명중했는지를 확인하고, 그 결과를 반환한다.
FServerSideRewindResult UDelayProcessingComponent::ProjectileConfirmHit(const FFramePackage& Package, ALetsShootCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime)
{
	// 현재 캐릭터의 히트박스 위치를 캐싱
	FFramePackage CurrentFrame;
	CacheBoxPositions(HitCharacter, CurrentFrame);

	// 이전 프레임 패키지의 상태로 히트박스를 이동
	MoveBoxes(HitCharacter, Package);

	// 캐릭터의 메쉬 충돌을 비활성화
	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::NoCollision);

	// 머리 히트박스의 충돌을 활성화
	UBoxComponent* HeadBox = HitCharacter->HitCollisionBoxes[FName("head")];
	HeadBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	HeadBox->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);

	// 발사체 경로 예측을 위한 파라미터 설정
	FPredictProjectilePathParams PathParams;
	PathParams.bTraceWithCollision = true;
	PathParams.MaxSimTime = MaxRecordTime;
	PathParams.LaunchVelocity = InitialVelocity;
	PathParams.StartLocation = TraceStart;
	PathParams.SimFrequency = 15.f;
	PathParams.ProjectileRadius = 5.f;
	PathParams.TraceChannel = ECC_HitBox;
	PathParams.ActorsToIgnore.Add(GetOwner());

	FPredictProjectilePathResult PathResult;

	// 발사체 경로를 예측하여 머리를 명중했는지 확인
	UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult);
	if (PathResult.HitResult.bBlockingHit) // 머리를 명중했을 경우, 이른 종료
	{
		ResetHitBoxes(HitCharacter, CurrentFrame);
		EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
		return FServerSideRewindResult{ true, true };
	}
	else // 머리를 명중하지 않았을 경우, 다른 히트박스를 확인
	{
		for (auto& HitBoxPair : HitCharacter->HitCollisionBoxes)
		{
			if (HitBoxPair.Value != nullptr)
			{
				HitBoxPair.Value->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				HitBoxPair.Value->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);
			}
		}

		// 다른 히트박스를 명중했는지 확인
		UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult);
		if (PathResult.HitResult.bBlockingHit)
		{
			ResetHitBoxes(HitCharacter, CurrentFrame);
			EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
			return FServerSideRewindResult{ true, false };
		}
	}

	// 명중하지 않았을 경우, 히트박스를 리셋하고 메쉬 충돌을 다시 활성화
	ResetHitBoxes(HitCharacter, CurrentFrame);
	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
	return FServerSideRewindResult{ false, false };
}


// 렉 보상 요소에서 샷건의 명중을 확인하는 메서드입니다.
// 해당 메서드는 주어진 프레임 패키지들, 발사 시작 위치, 명중 위치들을 이용하여 
// 샷건이 각 캐릭터를 명중했는지를 확인하고, 그 결과를 반환합니다.
FShotgunServerSideRewindResult UDelayProcessingComponent::ShotgunConfirmHit(const TArray<FFramePackage>& FramePackages, const FVector_NetQuantize& TraceStart, const TArray<FVector_NetQuantize>& HitLocations)
{
	// 프레임 패키지의 캐릭터들이 유효한지 확인
	for (auto& Frame : FramePackages)
	{
		if (Frame.Character == nullptr) return FShotgunServerSideRewindResult();
	}

	FShotgunServerSideRewindResult ShotgunResult;
	TArray<FFramePackage> CurrentFrames;

	// 각 캐릭터의 히트박스 위치를 캐싱하고 이전 프레임 패키지의 상태로 이동
	for (auto& Frame : FramePackages)
	{
		FFramePackage CurrentFrame;
		CurrentFrame.Character = Frame.Character;
		CacheBoxPositions(Frame.Character, CurrentFrame);
		MoveBoxes(Frame.Character, Frame);
		EnableCharacterMeshCollision(Frame.Character, ECollisionEnabled::NoCollision);
		CurrentFrames.Add(CurrentFrame);
	}

	// 각 캐릭터의 머리 히트박스의 충돌을 활성화
	for (auto& Frame : FramePackages)
	{
		UBoxComponent* HeadBox = Frame.Character->HitCollisionBoxes[FName("head")];
		HeadBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		HeadBox->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);
	}

	UWorld* World = GetWorld();

	// 머리 샷을 확인하기 위한 라인 트레이스를 수행
	for (auto& HitLocation : HitLocations)
	{
		FHitResult ConfirmHitResult;
		const FVector TraceEnd = TraceStart + (HitLocation - TraceStart) * 1.25f;
		if (World)
		{
			World->LineTraceSingleByChannel(
				ConfirmHitResult,
				TraceStart,
				TraceEnd,
				ECC_HitBox
			);
			ALetsShootCharacter* LetsShootCharacter = Cast<ALetsShootCharacter>(ConfirmHitResult.GetActor());
			if (LetsShootCharacter)
			{
				if (ShotgunResult.HeadShots.Contains(LetsShootCharacter))
				{
					ShotgunResult.HeadShots[LetsShootCharacter]++;
				}
				else
				{
					ShotgunResult.HeadShots.Emplace(LetsShootCharacter, 1);
				}
			}
		}
	}

	// 모든 히트박스의 충돌을 활성화하고, 머리 히트박스의 충돌은 비활성화
	for (auto& Frame : FramePackages)
	{
		for (auto& HitBoxPair : Frame.Character->HitCollisionBoxes)
		{
			if (HitBoxPair.Value != nullptr)
			{
				HitBoxPair.Value->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				HitBoxPair.Value->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);
			}
		}
		UBoxComponent* HeadBox = Frame.Character->HitCollisionBoxes[FName("head")];
		HeadBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// 몸통 샷을 확인하기 위한 라인 트레이스를 수행
	for (auto& HitLocation : HitLocations)
	{
		FHitResult ConfirmHitResult;
		const FVector TraceEnd = TraceStart + (HitLocation - TraceStart) * 1.25f;
		if (World)
		{
			World->LineTraceSingleByChannel(
				ConfirmHitResult,
				TraceStart,
				TraceEnd,
				ECC_HitBox
			);
			ALetsShootCharacter* LetsShootCharacter = Cast<ALetsShootCharacter>(ConfirmHitResult.GetActor());
			if (LetsShootCharacter)
			{
				if (ShotgunResult.BodyShots.Contains(LetsShootCharacter))
				{
					ShotgunResult.BodyShots[LetsShootCharacter]++;
				}
				else
				{
					ShotgunResult.BodyShots.Emplace(LetsShootCharacter, 1);
				}
			}
		}
	}

	// 히트박스를 원래 상태로 리셋하고, 메쉬 충돌을 다시 활성화
	for (auto& Frame : CurrentFrames)
	{
		ResetHitBoxes(Frame.Character, Frame);
		EnableCharacterMeshCollision(Frame.Character, ECollisionEnabled::QueryAndPhysics);
	}

	// 최종 명중 결과를 반환
	return ShotgunResult;
}


// HitCharacter의 HitCollisionBoxes에 대한 박스의 위치, 회전, 크기 정보를 저장합니다.
void UDelayProcessingComponent::CacheBoxPositions(ALetsShootCharacter* HitCharacter, FFramePackage& OutFramePackage)
{
	// HitCharacter가 유효한지 확인합니다.
	if (HitCharacter == nullptr) return;

	// HitCharacter의 HitCollisionBoxes에 대해 반복합니다.
	for (auto& HitBoxPair : HitCharacter->HitCollisionBoxes)
	{
		// HitBoxPair의 값이 유효한지 확인합니다.
		if (HitBoxPair.Value != nullptr)
		{
			// HitBox의 위치, 회전, 크기 정보를 저장합니다.
			FBoxInformation BoxInfo;
			BoxInfo.Location = HitBoxPair.Value->GetComponentLocation();
			BoxInfo.Rotation = HitBoxPair.Value->GetComponentRotation();
			BoxInfo.BoxExtent = HitBoxPair.Value->GetScaledBoxExtent();

			// OutFramePackage의 HitBoxInfo에 정보를 추가합니다.
			OutFramePackage.HitBoxInfo.Add(HitBoxPair.Key, BoxInfo);
		}
	}
}

// HitCharacter의 HitCollisionBoxes에 대한 정보를 사용하여 박스의 위치, 회전, 크기를 이동시킵니다.
void UDelayProcessingComponent::MoveBoxes(ALetsShootCharacter* HitCharacter, const FFramePackage& Package)
{
	// HitCharacter가 유효한지 확인합니다.
	if (HitCharacter == nullptr) return;

	// HitCharacter의 HitCollisionBoxes에 대해 반복합니다.
	for (auto& HitBoxPair : HitCharacter->HitCollisionBoxes)
	{
		// HitBoxPair의 값이 유효한지 확인합니다.
		if (HitBoxPair.Value != nullptr)
		{
			// Package의 정보를 사용하여 HitBox의 위치, 회전, 크기를 이동시킵니다.
			HitBoxPair.Value->SetWorldLocation(Package.HitBoxInfo[HitBoxPair.Key].Location);
			HitBoxPair.Value->SetWorldRotation(Package.HitBoxInfo[HitBoxPair.Key].Rotation);
			HitBoxPair.Value->SetBoxExtent(Package.HitBoxInfo[HitBoxPair.Key].BoxExtent);
		}
	}
}

// HitCharacter의 HitCollisionBoxes에 대한 정보를 사용하여 박스의 위치, 회전, 크기를 재설정하고 충돌을 비활성화합니다.
void UDelayProcessingComponent::ResetHitBoxes(ALetsShootCharacter* HitCharacter, const FFramePackage& Package)
{
	// HitCharacter가 유효한지 확인합니다.
	if (HitCharacter == nullptr) return;

	// HitCharacter의 HitCollisionBoxes에 대해 반복합니다.
	for (auto& HitBoxPair : HitCharacter->HitCollisionBoxes)
	{
		// HitBoxPair의 값이 유효한지 확인합니다.
		if (HitBoxPair.Value != nullptr)
		{
			// Package의 정보를 사용하여 HitBox의 위치, 회전, 크기를 재설정하고 충돌을 비활성화합니다.
			HitBoxPair.Value->SetWorldLocation(Package.HitBoxInfo[HitBoxPair.Key].Location);
			HitBoxPair.Value->SetWorldRotation(Package.HitBoxInfo[HitBoxPair.Key].Rotation);
			HitBoxPair.Value->SetBoxExtent(Package.HitBoxInfo[HitBoxPair.Key].BoxExtent);
			HitBoxPair.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

// HitCharacter와 HitCharacter의 Mesh의 충돌을 활성화 또는 비활성화합니다.
void UDelayProcessingComponent::EnableCharacterMeshCollision(ALetsShootCharacter* HitCharacter, ECollisionEnabled::Type CollisionEnabled)
{
	// HitCharacter와 HitCharacter의 Mesh가 유효한지 확인합니다.
	if (HitCharacter && HitCharacter->GetMesh())
	{
		// HitCharacter의 Mesh의 충돌을 활성화 또는 비활성화합니다.
		HitCharacter->GetMesh()->SetCollisionEnabled(CollisionEnabled);
	}
}


// FFramePackage에 저장된 HitBoxInfo를 사용하여 디버그 박스를 그립니다. 디버깅 목적으로 사용됩니다.
void UDelayProcessingComponent::ShowFramePackage(const FFramePackage& Package, const FColor& Color)
{
	// FFramePackage에 저장된 HitBoxInfo를 반복하여 디버그 박스를 그립니다.
	for (auto& BoxInfo : Package.HitBoxInfo)
	{
		DrawDebugBox(
			GetWorld(),
			BoxInfo.Value.Location,
			BoxInfo.Value.BoxExtent,
			FQuat(BoxInfo.Value.Rotation),
			Color,
			false,
			4.f
		);
	}
}

// 히트된 캐릭터에 대한 서버 측 재생을 수행하고, 히트 결과를 확인합니다.
FServerSideRewindResult UDelayProcessingComponent::ServerSideRewind(ALetsShootCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime)
{
	// HitCharacter에 대해 확인할 프레임을 가져옵니다.
	FFramePackage FrameToCheck = GetFrameToCheck(HitCharacter, HitTime);
	// ConfirmHit 함수를 호출하여 히트 확인 결과를 반환합니다.
	return ConfirmHit(FrameToCheck, HitCharacter, TraceStart, HitLocation);
}

// 투사체에 대한 서버 측 재생을 수행하고, 히트 결과를 확인합니다.
FServerSideRewindResult UDelayProcessingComponent::ProjectileServerSideRewind(ALetsShootCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime)
{
	// HitCharacter에 대해 확인할 프레임을 가져옵니다.
	FFramePackage FrameToCheck = GetFrameToCheck(HitCharacter, HitTime);
	// ProjectileConfirmHit 함수를 호출하여 히트 확인 결과를 반환합니다.
	return ProjectileConfirmHit(FrameToCheck, HitCharacter, TraceStart, InitialVelocity, HitTime);
}

// 샷건에 대한 서버 측 재생을 수행하고, 히트 결과를 확인합니다. HitCharacters에 포함된 모든 캐릭터에 대해 재생을 수행합니다.
FShotgunServerSideRewindResult UDelayProcessingComponent::ShotgunServerSideRewind(const TArray<ALetsShootCharacter*>& HitCharacters, const FVector_NetQuantize& TraceStart, const TArray<FVector_NetQuantize>& HitLocations, float HitTime)
{
	TArray<FFramePackage> FramesToCheck;
	// HitCharacters에 대해 확인할 프레임을 모두 가져옵니다.
	for (ALetsShootCharacter* HitCharacter : HitCharacters)
	{
		FramesToCheck.Add(GetFrameToCheck(HitCharacter, HitTime));
	}

	// ShotgunConfirmHit 함수를 호출하여 히트 확인 결과를 반환합니다.
	return ShotgunConfirmHit(FramesToCheck, TraceStart, HitLocations);
}


// 착시를 감지하려는 프레임 패키지를 반환하는 메서드입니다.
// HitCharacter: 감지할 히트 캐릭터의 포인터
// HitTime: 히트의 정확한 시간
FFramePackage UDelayProcessingComponent::GetFrameToCheck(ALetsShootCharacter* HitCharacter, float HitTime)
{
	// HitCharacter가 유효하지 않은 경우 빈 프레임 패키지를 반환합니다.
	bool bReturn =
		HitCharacter == nullptr ||
		HitCharacter->GetLagCompensation() == nullptr ||
		HitCharacter->GetLagCompensation()->FrameHistory.GetHead() == nullptr ||
		HitCharacter->GetLagCompensation()->FrameHistory.GetTail() == nullptr;
	if (bReturn) return FFramePackage();

	// 검증을 위해 확인할 프레임 패키지를 생성합니다.
	FFramePackage FrameToCheck;
	bool bShouldInterpolate = true;

	// HitCharacter의 프레임 히스토리를 가져옵니다.
	const TDoubleLinkedList<FFramePackage>& History = HitCharacter->GetLagCompensation()->FrameHistory;
	const float OldestHistoryTime = History.GetTail()->GetValue().Time;
	const float NewestHistoryTime = History.GetHead()->GetValue().Time;

	// 히트 타임이 가장 오래된 히스토리 시간보다 이전인 경우 (너무 많은 지연으로 SSR 분석이 불가능함)
	if (OldestHistoryTime > HitTime)
	{
		return FFramePackage();
	}

	// HitTime이 가장 오래된 히스토리 시간과 같은 경우
	if (OldestHistoryTime == HitTime)
	{
		FrameToCheck = History.GetTail()->GetValue();
		bShouldInterpolate = false;
	}

	// HitTime이 가장 최근의 히스토리 시간보다 같거나 이전인 경우
	if (NewestHistoryTime <= HitTime)
	{
		FrameToCheck = History.GetHead()->GetValue();
		bShouldInterpolate = false;
	}

	TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* Younger = History.GetHead();
	TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* Older = Younger;

	// Older가 HitTime보다 여전히 더 이른 시간인지 확인합니다.
	while (Older->GetValue().Time > HitTime)
	{
		// OlderTime < HitTime < YoungerTime이 되도록 이동합니다.
		if (Older->GetNextNode() == nullptr) break;
		Older = Older->GetNextNode();
		if (Older->GetValue().Time > HitTime)
		{
			Younger = Older;
		}
	}

	// 극히 드물겠지만, 찾고자 하는 프레임 시간이 정확히 일치하는 경우
	if (Older->GetValue().Time == HitTime)
	{
		FrameToCheck = Older->GetValue();
		bShouldInterpolate = false;
	}

	// 프레임 시간이 사이에 있는 경우 Younger와 Older 사이에서 선형 보간을 수행합니다.
	if (bShouldInterpolate)
	{
		FrameToCheck = InterpBetweenFrames(Older->GetValue(), Younger->GetValue(), HitTime);
	}

	// FrameToCheck의 캐릭터를 입력된 HitCharacter로 설정합니다.
	FrameToCheck.Character = HitCharacter;
	return FrameToCheck;
}


// 서버에 스코어 요청을 전달하는 메서드입니다.
// HitCharacter: 적중시킨 캐릭터의 포인터
// TraceStart: 팅귤레이캐스트의 시작 위치
// HitLocation: 주된 히트 위치
// HitTime: 히트가 발생한 시간
void UDelayProcessingComponent::ServerScoreRequest_Implementation(ALetsShootCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime)
{
	// 서버 측에서 착시를 감지하고 히트 여부를 확인합니다.
	FServerSideRewindResult Confirm = ServerSideRewind(HitCharacter, TraceStart, HitLocation, HitTime);

	// 히트 여부와 다른 조건이 충족되면 피해를 입힙니다.
	if (Character && HitCharacter && Character->GetEquippedWeapon() && Confirm.bHitConfirmed)
	{
		// 헤드샷 여부에 따른 데미지를 계산합니다.
		const float Damage = Confirm.bHeadShot ? Character->GetEquippedWeapon()->GetHeadShotDamage() : Character->GetEquippedWeapon()->GetDamage();

		// 피해를 적용합니다.
		UGameplayStatics::ApplyDamage(
			HitCharacter,
			Damage,
			Character->Controller,
			Character->GetEquippedWeapon(),
			UDamageType::StaticClass()
		);
	}
}

// 발사체를 사용하여 서버 스코어 요청을 처리하는 메서드입니다.
// HitCharacter: 발사체를 적중시킨 캐릭터의 포인터
// TraceStart: 트레이스 시작 위치
// InitialVelocity: 발사체의 초기 속도
// HitTime: 히트가 발생한 시간
void UDelayProcessingComponent::ProjectileServerScoreRequest_Implementation(ALetsShootCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime)
{
	// 발사체를 사용하여 서버 측에서 착시를 감지하고 히트 여부를 확인합니다.
	FServerSideRewindResult Confirm = ProjectileServerSideRewind(HitCharacter, TraceStart, InitialVelocity, HitTime);

	// 히트 여부와 다른 조건이 충족되면 피해를 입힙니다.
	if (&& HitCharacter && Confirm.bHitConfirmed && Character->GetEquippedWeapon())
	{
		// 헤드샷 여부에 따른 데미지를 계산합니다.
		const float Damage = Confirm.bHeadShot ? Character->GetEquippedWeapon()->GetHeadShotDamage() : Character->GetEquippedWeapon()->GetDamage();

		// 피해를 적용합니다.
		UGameplayStatics::ApplyDamage(
			HitCharacter,
			Damage,
			Character->Controller,
			Character->GetEquippedWeapon(),
			UDamageType::StaticClass()
		);
	}
}


// 샷건 서버 스코어 요청을 처리하는 메서드입니다.
// HitCharacters: 적중시킨 캐릭터의 포인터 배열
// TraceStart: 트레이스 시작 위치
// HitLocations: 주된 히트 위치의 배열
// HitTime: 히트가 발생한 시간
void UDelayProcessingComponent::ShotgunServerScoreRequest_Implementation(const TArray<ALetsShootCharacter*>& HitCharacters, const FVector_NetQuantize& TraceStart, const TArray<FVector_NetQuantize>& HitLocations, float HitTime)
{
	// 샷건을 사용하여 서버 측에서 착시를 감지하고 히트 여부를 확인합니다.
	FShotgunServerSideRewindResult Confirm = ShotgunServerSideRewind(HitCharacters, TraceStart, HitLocations, HitTime);

	// 각각의 적중 캐릭터에 대해서
	for (auto& HitCharacter : HitCharacters)
	{
		// 캐릭터, 무기, 장착 무기가 유효한지 확인합니다.
		if (HitCharacter == nullptr || HitCharacter->GetEquippedWeapon() == nullptr || Character == nullptr) continue;

		float TotalDamage = 0.f;

		// 헤드샷이 발생한 경우 데미지를 계산합니다.
		if (Confirm.HeadShots.Contains(HitCharacter))
		{
			float HeadShotDamage = Confirm.HeadShots[HitCharacter] * HitCharacter->GetEquippedWeapon()->GetHeadShotDamage();
			TotalDamage += HeadShotDamage;
		}

		// 몸통탄이 발생한 경우 데미지를 계산합니다.
		if (Confirm.BodyShots.Contains(HitCharacter))
		{
			float BodyShotDamage = Confirm.BodyShots[HitCharacter] * HitCharacter->GetEquippedWeapon()->GetDamage();
			TotalDamage += BodyShotDamage;
		}

		// 최종 피해를 적용합니다.
		UGameplayStatics::ApplyDamage(
			HitCharacter,
			TotalDamage,
			Character->Controller,
			HitCharacter->GetEquippedWeapon(),
			UDamageType::StaticClass()
		);
	}
}

// 컴포넌트를 틱할 때마다 호출되는 메서드입니다.
// DeltaTime: 이전 프레임 이후 지난 시간
// TickType: 틱하는 레벨
// ThisTickFunction: 이 틱 함수의 포인터
void UDelayProcessingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	// 부모 클래스의 TickComponent를 호출합니다.
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 매 프레임 패키지를 저장합니다.
	SaveFramePackage();
}


// 프레임별 패키지를 저장하는 메서드입니다.
void UDelayProcessingComponent::SaveFramePackage()
{
	if (Character == nullptr || !Character->HasAuthority()) return;

	// 프레임 기록이 빈 경우 현재 패키지를 기록합니다.
	if (FrameHistory.Num() <= 1)
	{
		FFramePackage ThisFrame;
		SaveFramePackage(ThisFrame);
		FrameHistory.AddHead(ThisFrame);
	}
	else
	{
		// 최대 기록 시간을 초과할 경우 오래된 기록을 삭제합니다.
		float HistoryLength = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
		while (HistoryLength > MaxRecordTime)
		{
			FrameHistory.RemoveNode(FrameHistory.GetTail());
			HistoryLength = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
		}

		// 현재 패키지를 프레임 기록에 추가합니다.
		FFramePackage ThisFrame;
		SaveFramePackage(ThisFrame);
		FrameHistory.AddHead(ThisFrame);

		// 디버그 목적으로 패키지를 표시합니다.
		// ShowFramePackage(ThisFrame, FColor::Red);
	}
}

// 패키지 정보를 저장하는 메서드입니다.
// Package: 저장할 패키지 정보
void UDelayProcessingComponent::SaveFramePackage(FFramePackage& Package)
{
	// 캐릭터를 가져온 후 패키지 정보를 채웁니다.
	Character = Character == nullptr ? Cast<ALetsShootCharacter>(GetOwner()) : Character;
	if (Character)
	{
		Package.Time = GetWorld()->GetTimeSeconds();
		Package.Character = Character;

		// 각 히트 박스 정보를 패키지에 추가합니다.
		for (auto& BoxPair : Character->HitCollisionBoxes)
		{
			FBoxInformation BoxInformation;
			BoxInformation.Location = BoxPair.Value->GetComponentLocation();
			BoxInformation.Rotation = BoxPair.Value->GetComponentRotation();
			BoxInformation.BoxExtent = BoxPair.Value->GetScaledBoxExtent();
			Package.HitBoxInfo.Add(BoxPair.Key, BoxInformation);
		}
	}
}


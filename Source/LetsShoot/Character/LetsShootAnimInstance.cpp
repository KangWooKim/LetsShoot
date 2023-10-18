// LetsShootAnimInstance.cpp

#include "LetsShootAnimInstance.h"
#include "LetsShootCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "LetsShoot/Weapon/Weapon.h"
#include "LetsShoot/LetsShootTypes/CombatState.h"

void ULetsShootAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	LetsShootCharacter = Cast<ALetsShootCharacter>(TryGetPawnOwner());
}

void ULetsShootAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (LetsShootCharacter == nullptr)
	{
		LetsShootCharacter = Cast<ALetsShootCharacter>(TryGetPawnOwner());
	}
	if (LetsShootCharacter == nullptr) return;

	// 계산에 필요한 변수들을 설정합니다.
	FVector Velocity = LetsShootCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	bIsInAir = LetsShootCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = LetsShootCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
	bWeaponEquipped = LetsShootCharacter->IsWeaponEquipped();
	EquippedWeapon = LetsShootCharacter->GetEquippedWeapon();
	bIsCrouched = LetsShootCharacter->bIsCrouched;
	bAiming = LetsShootCharacter->IsAiming();
	TurningInPlace = LetsShootCharacter->GetTurningInPlace();
	bRotateRootBone = LetsShootCharacter->ShouldRotateRootBone();
	bElimmed = LetsShootCharacter->IsElimmed();
	bHoldingTheFlag = LetsShootCharacter->IsHoldingTheFlag();

	// 좌우 이동에 따른 Yaw 오프셋을 계산합니다.
	FRotator AimRotation = LetsShootCharacter->GetBaseAimRotation();
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(LetsShootCharacter->GetVelocity());
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
	DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaTime, 6.f);
	YawOffset = DeltaRotation.Yaw;

	CharacterRotationLastFrame = CharacterRotation;
	CharacterRotation = LetsShootCharacter->GetActorRotation();
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
	const float Target = Delta.Yaw / DeltaTime;
	const float Interp = FMath::FInterpTo(Lean, Target, DeltaTime, 6.f);
	Lean = FMath::Clamp(Interp, -90.f, 90.f);

	AO_Yaw = LetsShootCharacter->GetAO_Yaw();
	AO_Pitch = LetsShootCharacter->GetAO_Pitch();

	// 무기가 장착되어 있고, 무기 메시와 캐릭터 메시가 존재하는 경우, 왼손의 위치와 회전을 계산합니다.
	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && LetsShootCharacter->GetMesh())
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		LetsShootCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		// 로컬 플레이어가 조종 중이라면, 오른손의 회전을 계산합니다.
		if (LetsShootCharacter->IsLocallyControlled())
		{
			bLocallyControlled = true;
			FTransform RightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("hand_r"), ERelativeTransformSpace::RTS_World);
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - LetsShootCharacter->GetHitTarget()));
			RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRotation, DeltaTime, 30.f);
		}
	}

	// FABRIK 사용 여부를 결정합니다.
	bUseFABRIK = LetsShootCharacter->GetCombatState() == ECombatState::ECS_Unoccupied;
	bool bFABRIKOverride = LetsShootCharacter->IsLocallyControlled() &&
		LetsShootCharacter->GetCombatState() != ECombatState::ECS_ThrowingGrenade &&
		LetsShootCharacter->bFinishedSwapping;
	if (bFABRIKOverride)
	{
		bUseFABRIK = !LetsShootCharacter->IsLocallyReloading();
	}

	bUseAimOffsets = LetsShootCharacter->GetCombatState() == ECombatState::ECS_Unoccupied && !LetsShootCharacter->GetDisableGameplay();
	bTransformRightHand = LetsShootCharacter->GetCombatState() == ECombatState::ECS_Unoccupied && !LetsShootCharacter->GetDisableGameplay();
}

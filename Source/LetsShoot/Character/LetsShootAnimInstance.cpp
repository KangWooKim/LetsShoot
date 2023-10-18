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

	// ��꿡 �ʿ��� �������� �����մϴ�.
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

	// �¿� �̵��� ���� Yaw �������� ����մϴ�.
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

	// ���Ⱑ �����Ǿ� �ְ�, ���� �޽ÿ� ĳ���� �޽ð� �����ϴ� ���, �޼��� ��ġ�� ȸ���� ����մϴ�.
	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && LetsShootCharacter->GetMesh())
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		LetsShootCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		// ���� �÷��̾ ���� ���̶��, �������� ȸ���� ����մϴ�.
		if (LetsShootCharacter->IsLocallyControlled())
		{
			bLocallyControlled = true;
			FTransform RightHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("hand_r"), ERelativeTransformSpace::RTS_World);
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - LetsShootCharacter->GetHitTarget()));
			RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRotation, DeltaTime, 30.f);
		}
	}

	// FABRIK ��� ���θ� �����մϴ�.
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

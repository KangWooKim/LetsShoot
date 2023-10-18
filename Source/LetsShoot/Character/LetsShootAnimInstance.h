// ULetsShootAnimInstance.h

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "LetsShootCharacter.h"
#include "Weapon.h"
#include "LetsShootEnums.h"
#include "LetsShootAnimInstance.generated.h"

/**
 * �ִϸ��̼� �ν��Ͻ� Ŭ�����Դϴ�.
 */
UCLASS()
class LETSSHOOT_API ULetsShootAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	/**
	 * �ִϸ��̼� �ʱ�ȭ�� �����ϴ� �Լ��Դϴ�.
	 */
	virtual void NativeInitializeAnimation() override;

	/**
	 * �ִϸ��̼� ������Ʈ�� �����ϴ� �Լ��Դϴ�.
	 */
	virtual void NativeUpdateAnimation(float DeltaTime) override;

private:
	/**
	 * LetsShootCharacter�� �������Դϴ�.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
		ALetsShootCharacter* LetsShootCharacter;

	/**
	 * ĳ������ �ӵ��Դϴ�.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float Speed;

	/**
	 * ���߿� �ִ��� ���θ� ��Ÿ���ϴ�.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool bIsInAir;

	/**
	 * ���� ������ ���θ� ��Ÿ���ϴ�.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool bIsAccelerating;

	/**
	 * ���Ⱑ �����Ǿ� �ִ��� ���θ� ��Ÿ���ϴ�.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool bWeaponEquipped;

	/**
	 * ������ ������ �������Դϴ�.
	 */
	class AWeapon* EquippedWeapon;

	/**
	 * �ɾ� �ִ��� ���θ� ��Ÿ���ϴ�.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool bIsCrouched;

	/**
	 * ���� ������ ���θ� ��Ÿ���ϴ�.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool bAiming;

	/**
	 * Yaw ������ �������Դϴ�.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float YawOffset;

	/**
	 * Lean ���� ��Ÿ���ϴ�.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float Lean;

	/**
	 * ���� �������� ĳ���� ȸ���Դϴ�.
	 */
	FRotator CharacterRotationLastFrame;

	/**
	 * ���� �������� ĳ���� ȸ���Դϴ�.
	 */
	FRotator CharacterRotation;

	/**
	 * ȸ�� ��ȭ���Դϴ�.
	 */
	FRotator DeltaRotation;

	/**
	 * Yaw �����Դϴ�.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float AO_Yaw;

	/**
	 * Pitch �����Դϴ�.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float AO_Pitch;

	/**
	 * ���� ���� ��ȯ �����Դϴ�.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		FTransform LeftHandTransform;

	/**
	 * ���ڸ����� ȸ���ϴ� ���� �����Դϴ�.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		ETurningInPlace TurningInPlace;

	/**
	 * ������ ���� ȸ���Դϴ�.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		FRotator RightHandRotation;

	/**
	 * ���� ��Ʈ���� �ϴ��� ���θ� ��Ÿ���ϴ�.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool bLocallyControlled;

	/**
	 * ��Ʈ ���� ȸ���ϴ��� ���θ� ��Ÿ���ϴ�.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool bRotateRootBone;

	/**
	 * ���ŵ� �������� ���θ� ��Ÿ���ϴ�.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool bElimmed;

	/**
	 * FABRIK�� ����ϴ��� ���θ� ��Ÿ���ϴ�.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool bUseFABRIK;

	/**
	 * Aim Offset�� ����ϴ��� ���θ� ��Ÿ���ϴ�.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool bUseAimOffsets;

	/**
	 * ������ ���� ��ȯ�ϴ��� ���θ� ��Ÿ���ϴ�.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool bTransformRightHand;

	/**
	 * ����� ��� �ִ��� ���θ� ��Ÿ���ϴ�.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool bHoldingTheFlag;
};

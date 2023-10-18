// ULetsShootAnimInstance.h

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "LetsShootCharacter.h"
#include "Weapon.h"
#include "LetsShootEnums.h"
#include "LetsShootAnimInstance.generated.h"

/**
 * 애니메이션 인스턴스 클래스입니다.
 */
UCLASS()
class LETSSHOOT_API ULetsShootAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	/**
	 * 애니메이션 초기화를 수행하는 함수입니다.
	 */
	virtual void NativeInitializeAnimation() override;

	/**
	 * 애니메이션 업데이트를 수행하는 함수입니다.
	 */
	virtual void NativeUpdateAnimation(float DeltaTime) override;

private:
	/**
	 * LetsShootCharacter의 포인터입니다.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
		ALetsShootCharacter* LetsShootCharacter;

	/**
	 * 캐릭터의 속도입니다.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float Speed;

	/**
	 * 공중에 있는지 여부를 나타냅니다.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool bIsInAir;

	/**
	 * 가속 중인지 여부를 나타냅니다.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool bIsAccelerating;

	/**
	 * 무기가 장착되어 있는지 여부를 나타냅니다.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool bWeaponEquipped;

	/**
	 * 장착된 무기의 포인터입니다.
	 */
	class AWeapon* EquippedWeapon;

	/**
	 * 앉아 있는지 여부를 나타냅니다.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool bIsCrouched;

	/**
	 * 조준 중인지 여부를 나타냅니다.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool bAiming;

	/**
	 * Yaw 각도의 오프셋입니다.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float YawOffset;

	/**
	 * Lean 값을 나타냅니다.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float Lean;

	/**
	 * 이전 프레임의 캐릭터 회전입니다.
	 */
	FRotator CharacterRotationLastFrame;

	/**
	 * 현재 프레임의 캐릭터 회전입니다.
	 */
	FRotator CharacterRotation;

	/**
	 * 회전 변화량입니다.
	 */
	FRotator DeltaRotation;

	/**
	 * Yaw 각도입니다.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float AO_Yaw;

	/**
	 * Pitch 각도입니다.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		float AO_Pitch;

	/**
	 * 왼쪽 손의 변환 정보입니다.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		FTransform LeftHandTransform;

	/**
	 * 제자리에서 회전하는 동작 상태입니다.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		ETurningInPlace TurningInPlace;

	/**
	 * 오른쪽 손의 회전입니다.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		FRotator RightHandRotation;

	/**
	 * 로컬 컨트롤을 하는지 여부를 나타냅니다.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool bLocallyControlled;

	/**
	 * 루트 본을 회전하는지 여부를 나타냅니다.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool bRotateRootBone;

	/**
	 * 제거된 상태인지 여부를 나타냅니다.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool bElimmed;

	/**
	 * FABRIK를 사용하는지 여부를 나타냅니다.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool bUseFABRIK;

	/**
	 * Aim Offset을 사용하는지 여부를 나타냅니다.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool bUseAimOffsets;

	/**
	 * 오른쪽 손을 변환하는지 여부를 나타냅니다.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool bTransformRightHand;

	/**
	 * 깃발을 들고 있는지 여부를 나타냅니다.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
		bool bHoldingTheFlag;
};

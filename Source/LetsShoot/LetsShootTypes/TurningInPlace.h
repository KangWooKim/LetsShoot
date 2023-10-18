#pragma once

UENUM(BlueprintType)
enum class ETurningInPlace : uint8
{
	ETIP_Left UMETA(DisplayName = "Turning Left"),  // 회전 상태: 좌회전 중
	ETIP_Right UMETA(DisplayName = "Turning Right"),  // 회전 상태: 우회전 중
	ETIP_NotTurning UMETA(DisplayName = "Not Turning"),  // 회전 상태: 회전하지 않음

	ETIP_MAX UMETA(DisplayName = "Default MAX")  // 회전 상태: 기본 최대값
};

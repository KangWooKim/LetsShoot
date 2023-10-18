#pragma once

UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),  // 상태: 미사용 중
	ECS_Reloading UMETA(DisplayName = "Reloading"),  // 상태: 재장전 중
	ECS_ThrowingGrenade UMETA(DisplayName = "Throwing Grenade"),  // 상태: 수류탄 던지기 중
	ECS_SwappingWeapons UMETA(DisplayName = "Swapping Weapons"),  // 상태: 무기 교체 중

	ECS_MAX UMETA(DisplayName = "Default MAX")  // 상태: 기본 최대값
};

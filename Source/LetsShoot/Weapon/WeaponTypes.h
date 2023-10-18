#pragma once

#define TRACE_LENGTH 80000.f

#define CUSTOM_DEPTH_PURPLE 250
#define CUSTOM_DEPTH_BLUE 251
#define CUSTOM_DEPTH_TAN 252

UENUM(BlueprintType)
// 무기의 종류를 나타내는 열거형
enum class EWeaponType : uint8
{
	EWT_AssaultRifle UMETA(DisplayName = "Assault Rifle"),       // 돌격소총
	EWT_RocketLauncher UMETA(DisplayName = "Rocket Launcher"),   // 로켓 발사기
	EWT_Pistol UMETA(DisplayName = "Pistol"),                    // 권총
	EWT_SubmachineGun UMETA(DisplayName = "Submachine Gun"),     // 기관단총
	EWT_Shotgun UMETA(DisplayName = "Shotgun"),                  // 산탄총
	EWT_SniperRifle UMETA(DisplayName = "Sniper Rifle"),         // 저격소총
	EWT_GrenadeLauncher UMETA(DisplayName = "Grenade Launcher"), // 수류탄 발사기
	EWT_Flag UMETA(DisplayName = "Flag"),                        // 깃발

	EWT_MAX UMETA(DisplayName = "DefaultMAX")                    // 기본 최대값
};

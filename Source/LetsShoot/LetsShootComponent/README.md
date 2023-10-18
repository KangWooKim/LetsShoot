# BuffComponent.h

### 1. UBuffComponent::Heal(float HealAmount, float HealingTime): 

이 함수는 주어진 시간 동안 캐릭터의 체력을 점진적으로 회복하는 버프를 설정합니다.

### 2. UBuffComponent::ReplenishShield(float ShieldAmount, float ReplenishTime): 

이 함수는 주어진 시간 동안 캐릭터의 방패를 점진적으로 회복하는 버프를 설정합니다.

### 3. UBuffComponent::HealRampUp(float DeltaTime): 

이 함수는 "Heal" 버프를 실행하는 메서드입니다. 이는 프레임당 호출되어 캐릭터의 체력을 점진적으로 회복합니다.

### 4. UBuffComponent::ShieldRampUp(float DeltaTime): 

이 함수는 "ReplenishShield" 버프를 실행하는 메서드입니다. 이는 프레임당 호출되어 캐릭터의 방패를 점진적으로 회복합니다.

### 5. UBuffComponent::SetInitialSpeeds(float BaseSpeed, float CrouchSpeed): 

이 함수는 캐릭터의 원래 이동 속도와 웅크린 상태에서의 이동 속도를 설정합니다.

### 6. UBuffComponent::SetInitialJumpVelocity(float Velocity): 

이 함수는 캐릭터의 원래 점프 속도를 설정합니다.

### 7. UBuffComponent::BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime): 

이 함수는 주어진 시간 동안 캐릭터의 이동 속도와 웅크린 상태에서의 이동 속도를 일시적으로 증가시킵니다.

### 8. UBuffComponent::ResetSpeeds(): 

이 함수는 "BuffSpeed" 버프가 끝난 후 캐릭터의 이동 속도를 원래대로 되돌립니다.

### 9. UBuffComponent::BuffJump(float BuffJumpVelocity, float BuffTime): 

이 함수는 주어진 시간 동안 캐릭터의 점프 속도를 일시적으로 증가시킵니다.

### 10. UBuffComponent::ResetJump(): 

이 함수는 "BuffJump" 버프가 끝난 후 캐릭터의 점프 속도를 원래대로 되돌립니다.

### 11. UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction): 

이 함수는 매 프레임마다 호출되며, 이때 HealRampUp 및 ShieldRampUp 함수를 호출하여 체력 및 방패 회복 버프를 적용합니다.

이 클래스는 게임 캐릭터의 능력치를 임시적으로 증가시키는데 사용됩니다.

---

# CombatComponent.h

### UCombatComponent::UCombatComponent() : 

이것은 생성자입니다. 이 메서드는 UCombatComponent 클래스의 새 인스턴스를 초기화합니다. 여기서는 BaseWalkSpeed와 AimWalkSpeed을 초기화합니다.

### UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const : 
  
  이 메서드는 네트워크 레플리케이션 시스템에 이 컴포넌트의 레플리케이션 속성을 알리는 데 사용됩니다. 레플리케이션은 네트워크 멀티플레이어 게임에서 중요한 개념입니다. 이 메서드는 서버에서 클라이언트로 상태를 동기화하는데 사용됩니다.

### UCombatComponent::ShotgunShellReload() : 
  
  캐릭터가 권한이 있을 경우, 샷건 탄약 값을 업데이트하는 메서드입니다.

### UCombatComponent::PickupAmmo(EWeaponType WeaponType, int32 AmmoAmount) : 
  
  이 메서드는 캐릭터가 탄약을 주웠을 때 호출됩니다. 주어진 무기 유형에 대한 탄약 양을 업데이트하고, 필요한 경우 즉시 재장전을 시작합니다.

### UCombatComponent::BeginPlay() : 
  
  이 메서드는 게임 시작 시 호출되며, 캐릭터의 움직임 속도와 카메라의 기본 시야를 설정합니다.

### UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) : 
  
  이 메서드는 프레임마다 호출되며, 캐릭터가 로컬로 제어되는 경우, 십자선 아래에 있는 물체를 추적하고 HUD 십자선을 설정하고 시야를 보간합니다.

### UCombatComponent::FireButtonPressed(bool bPressed) : 
  
  이 메서드는 불을 지휘하는 버튼이 눌렸을 때 호출됩니다. 눌린 경우, Fire 메서드를 호출합니다.

### UCombatComponent::Fire(), UCombatComponent::FireProjectileWeapon(), UCombatComponent::FireHitScanWeapon(), UCombatComponent::FireShotgun() : 
  메서드는 캐릭터가 무기를 발사할 때 사용되며, 무기의 종류에 따라 다른 메서드가 호출됩니다.

### UCombatComponent::StartFireTimer(), UCombatComponent::FireTimerFinished() : 
  이 메서드들은 발사 간의 지연을 관리합니다.
  
### void UCombatComponent::Fire(): 
  이 메서드는 캐릭터가 무기를 쏠 수 있는지 검사한 후, 무기에 따른 적절한 발사 방법을 실행합니다. 발사 후에는 StartFireTimer를 호출하여 발사 대기 시간을 설정합니다.

### void UCombatComponent::FireProjectileWeapon(): 
  이 메서드는 투사체 무기를 발사합니다. 이 메서드는 서버에서 권한이 있는지 확인하고, 해당 무기가 흩뿌리기 방식을 사용하는지 여부에 따라 투사체가 명중할 위치를 조정합니다.

### void UCombatComponent::FireHitScanWeapon(): 
  이 메서드는 히트스캔 무기를 발사합니다. 투사체 무기를 발사하는 메서드와 유사하게 작동하지만, 히트스캔 무기는 즉시 목표에 명중하는 것을 시뮬레이트합니다.

### void UCombatComponent::FireShotgun(): 
  이 메서드는 샷건을 발사합니다. 이 메서드는 샷건의 특성에 따라 여러 탄환을 동시에 발사합니다.

### void UCombatComponent::StartFireTimer(): 
  이 메서드는 무기의 발사 대기 시간을 설정합니다. 설정된 시간이 지나면 FireTimerFinished 메서드가 호출됩니다.

### void UCombatComponent::FireTimerFinished(): 
  이 메서드는 발사 대기 시간이 지난 후 호출됩니다. 이 메서드는 캐릭터가 계속 발사하려고 하는지 확인하고, 캐릭터가 자동 무기를 장착하고 있으면 다시 발사합니다. 또한, 무기가 비었는지 확인하고 필요하면 재장전을 시도합니다.

### void UCombatComponent::ServerFire_Implementation(), bool UCombatComponent::ServerFire_Validate(): 
  이 두 메서드는 무기 발사를 위한 서버 사이드 유효성 검사 및 구현을 처리합니다.

### void UCombatComponent::MulticastFire_Implementation(): 
  이 메서드는 무기 발사를 클라이언트에게 다중 송신합니다.

### void UCombatComponent::ServerShotgunFire_Implementation(), bool UCombatComponent::ServerShotgunFire_Validate(): 
  이 두 메서드는 샷건 발사를 위한 서버 사이드 유효성 검사 및 구현을 처리합니다.

### void UCombatComponent::MulticastShotgunFire_Implementation(): 
  이 메서드는 샷건 발사를 클라이언트에게 다중 송신합니다.

### void UCombatComponent::LocalFire(): 
  이 메서드는 클라이언트에서 무기 발사를 처리합니다. 이 메서드는 캐릭터의 현재 상태와 무기의 상태에 따라 적절한 발사 방법을 실행하거나, 발사를 실패하게 됩니다.

### void UCombatComponent::Reload(): 
  이 메서드는 캐릭터가 무기를 재장전하도록 요청합니다. 재장전 요청이 유효하면 StartReloadTimer를 호출하여 재장전 대기 시간을 설정합니다.

### void UCombatComponent::StartReloadTimer(): 
  이 메서드는 재장전 대기 시간을 설정합니다. 설정된 시간이 지나면 ReloadTimerFinished 메서드가 호출됩니다.

### void UCombatComponent::ReloadTimerFinished(): 
  이 메서드는 재장전 대기 시간이 지난 후 호출됩니다. 재장전이 완료되면 무기의 탄약 상태를 업데이트합니다.

### void UCombatComponent::ServerReload_Implementation(), bool UCombatComponent::ServerReload_Validate(): 
  이 두 메서드는 재장전을 위한 서버 사이드 유효성 검사 및 구현을 처리합니다.

### void UCombatComponent::MulticastReload_Implementation(): 
  이 메서드는 재장전을 클라이언트에게 다중 송신합니다.

### void UCombatComponent::LocalReload(): 
  이 메서드는 클라이언트에서 재장전을 처리합니다. 이 메서드는 캐릭터의 현재 상태와 무기의 탄약 상태에 따라 적절한 재장전 방법을 실행하거나, 재장전을 실패하게 됩니다.

### void UCombatComponent::CancelReload():
  이 메서드는 캐릭터가 재장전을 중단하도록 요청합니다. 재장전 중단 요청이 유효하면 ReloadTimerFinished 메서드의 호출을 취소합니다.

### void UCombatComponent::ServerCancelReload_Implementation(), bool UCombatComponent::ServerCancelReload_Validate(): 
  이 두 메서드는 재장전 취소를 위한 서버 사이드 유효성 검사 및 구현을 처리합니다.

### void UCombatComponent::MulticastCancelReload_Implementation():
  이 메서드는 재장전 취소를 클라이언트에게 다중 송신합니다.

### void UCombatComponent::LocalCancelReload(): 
  이 메서드는 클라이언트에서 재장전 취소를 처리합니다. 이 메서드는 캐릭터의 현재 상태에 따라 적절한 재장전 취소 방법을 실행합니다.
  
### void UCombatComponent::UpdateAmmoValues: 
  장착된 무기의 탄창을 채우고, 잔여 탄약을 업데이트하는 역할을 합니다. 또한, HUD에 남은 탄약 수를 업데이트합니다.

### void UCombatComponent::UpdateShotgunAmmoValues: 
  샷건에 대한 탄약 값을 업데이트합니다. 이 메서드는 샷건의 특성 상 한 번에 한 발씩 장전되므로, 매번 탄약을 1 감소시키며 HUD를 업데이트합니다.

### void UCombatComponent::OnRep_Grenades: 
  그레네이드의 수를 HUD에 업데이트합니다.

### void UCombatComponent::JumpToShotgunEnd: 
  애니메이션 인스턴스를 사용해 샷건 장전의 마지막 부분으로 이동합니다.

### void UCombatComponent::ThrowGrenadeFinished: 
  그레네이드를 던지는 행동이 완료되었을 때 호출되어, 전투 상태를 'Unoccupied'로 변경하고, 무기를 다시 손에 장착합니다.

### void UCombatComponent::LaunchGrenade: 
  그레네이드를 던지는 로직을 처리합니다. 서버에 그레네이드 던지기를 요청합니다.

### void UCombatComponent::ServerLaunchGrenade_Implementation: 
  서버에서 그레네이드를 던지는 데 필요한 로직을 구현합니다. 그레네이드 객체를 생성하고, 월드에 스폰시킵니다.

### void UCombatComponent::OnRep_CombatState: 
  전투 상태가 변경될 때마다 호출됩니다. 전투 상태에 따라 적절한 행동을 실행합니다.

### void UCombatComponent::HandleReload: 
  캐릭터가 재장전 애니메이션을 재생합니다.

### void UCombatComponent::AmountToReload: 
  장착된 무기에 필요한 탄약 수를 계산합니다. 무기의 탄창 용량과 잔여 탄약 중에서 적은 것을 반환합니다.

### void UCombatComponent::ThrowGrenade: 
  그레네이드를 던지는 로직을 시작합니다. 그레네이드가 있고, 전투 상태가 'Unoccupied'이며, 무기가 장착되어 있는 경우에만 그레네이드를 던질 수 있습니다.

### void UCombatComponent::ServerLaunchGrenade_Implementation(const FVector_NetQuantize& Target): 
  서버에서 수행되며, 유효한 GrenadeClass와 Character가 있고, Character가 Grenade를 가지고 있을 때, Grenade를 발사합니다. 이 메서드는 Grenade의 시작 위치를 설정하고, 타겟 방향으로 Grenade를 회전시키며, 그 후 Grenade를 생성하고 발사합니다.

### void UCombatComponent::OnRep_CombatState(): 
  CombatState의 상태에 따라 다른 액션을 수행하는 메서드입니다. 각 상태(ECS_Reloading, ECS_Unoccupied, ECS_ThrowingGrenade, ECS_SwappingWeapons)에 따라 다른 캐릭터 애니메이션을 재생하거나 다른 액션을 수행합니다.

### void UCombatComponent::HandleReload(): 
  캐릭터가 있는 경우, 캐릭터의 재장전 애니메이션을 실행합니다.

### void UCombatComponent::AmountToReload(): 
  장착된 무기가 있는지 확인하고, 무기의 탄창에 얼마나 많은 탄약이 들어갈 수 있는지 계산합니다. 이 메서드는 탄약을 리로드할 때 얼마나 많은 탄약이 필요한지 결정하는데 사용됩니다.

### void UCombatComponent::ThrowGrenade(): 
  Grenade가 있는지 확인하고, 현재 CombatState가 Unoccupied이며 장착 무기가 있는지 확인합니다. 그 후 Grenade를 던지는 상태로 변경하고, 캐릭터가 Grenade 던지는 애니메이션을 재생합니다. 또한, 캐릭터가 서버에 있는지 확인하고, 서버에서 Grenade를 던집니다. 그 후, Grenade의 수를 업데이트하고 HUD를 업데이트합니다.

### void UCombatComponent::ServerThrowGrenade_Implementation(): 
  서버에서 실행되며, Grenade를 던지는 상태로 변경하고, Grenade 던지는 애니메이션을 재생합니다. 그 후 Grenade의 수를 줄이고 HUD를 업데이트합니다.

### void UCombatComponent::UpdateHUDGrenades(): 
  Controller를 설정하고, Controller가 있는 경우 Controller에 현재 Grenade 수를 설정합니다.

### void UCombatComponent::ShouldSwapWeapons(): 
  현재 장착된 무기와 보조 무기가 있는지 확인하고, 무기를 교체해야하는지 결정합니다.

### void UCombatComponent::ShowAttachedGrenade(bool bShowGrenade): 
  캐릭터가 Grenade를 가지고 있는 경우, 인자로 받은 값에 따라 Grenade를 보이게 하거나 숨깁니다.
  
---
  
# DelayProcessingComponent.h
  
이 클래스는 주로 네트워크 지연(Lag) 보상을 처리하기 위해 사용됩니다.

### UDelayProcessingComponent() 
  이것은 클래스의 생성자입니다. PrimaryComponentTick.bCanEverTick = true; 라인은 이 컴포넌트가 틱 이벤트(게임 엔진이 한 프레임을 그리기 위한 주요 루프)를 받을 수 있도록 설정합니다.

### BeginPlay() 
  이 메서드는 액터가 게임에서 시작되면 호출됩니다. 현재로서는 부모 클래스인 Super::BeginPlay()만 호출하고 있습니다.

### InterpBetweenFrames() 
  이 메서드는 두 프레임 패키지 사이를 보간하는 데 사용됩니다. 이는 주로 두 프레임 사이에서 객체의 위치와 회전을 보간하여 그 사이에 발생한 움직임을 재현하거나 예측하는 데 사용됩니다.

### ConfirmHit()
  이 메서드는 플레이어가 히트박스에 물리적 피해를 입혔는지 확인하는 데 사용됩니다. 해당 피해가 유효한지 여부를 결정하고, 피해가 발생했을 경우 해당 정보를 반환합니다.

먼저, 'head'라는 이름의 히트박스(일반적으로 캐릭터의 머리를 대표)의 충돌을 활성화하고, 충돌 채널을 설정합니다. 그런 다음 LineTraceSingleByChannel() 메서드를 사용하여 발사선을 그립니다. 이는 캐릭터의 'head' 히트박스에 히트가 있는지 확인하기 위한 것입니다. 히트가 발견되면 해당 정보를 반환하고 메서드를 종료합니다.

만약 'head' 히트박스에 히트가 없다면, 모든 다른 히트박스의 충돌을 활성화하고, 다시 한 번 선을 그려서 어떤 히트박스에 히트가 있는지 확인합니다. 히트가 있다면 해당 정보를 반환하고 메서드를 종료합니다.

마지막으로, 만약 어떠한 히트박스에도 히트가 없다면, 메서드는 히트가 없음을 반환하고 종료됩니다.
  
### ProjectileConfirmHit() 
  이 메서드는 투사체가 블래스터 캐릭터에 맞았는지를 확인하는 로직을 담당합니다. 이는 플레이어가 발사한 투사체가 목표물을 정확하게 맞췄는지를 판정하는 역할을 수행합니다.

투사체 경로의 예측을 위해 UGameplayStatics::PredictProjectilePath() 함수를 사용하고 있습니다. 이 함수는 주어진 파라미터에 따라 투사체의 경로를 예측하고, 그 경로에 있는 오브젝트와의 충돌 여부를 반환합니다. 'head' 히트박스에 히트가 있다면, 히트 정보를 반환하고 메서드를 종료합니다. 'head' 히트박스에 히트가 없다면, 다른 모든 히트박스의 충돌을 활성화하고 다시 한번 투사체 경로를 예측합니다. 만약 이번에 히트가 있다면, 해당 정보를 반환하고, 그렇지 않다면 히트가 없음을 반환합니다.

### ShotgunConfirmHit() 
  이 메서드는 샷건이 적 캐릭터에게 히트했는지를 확인하는 로직을 담당합니다. 샷건은 여러 발의 샷(탄환)을 동시에 발사하므로, 여러 캐릭터에 대한 히트를 처리해야 합니다.

먼저, 각 캐릭터에 대해 헤드박스에 히트가 있는지를 확인합니다. 헤드박스에 히트가 있다면 해당 캐릭터의 헤드샷 카운트를 증가시키고, 그렇지 않다면 새로운 헤드샷 카운트를 시작합니다. 이후, 모든 히트박스의 충돌을 활성화하고 헤드박스의 충돌을 비활성화합니다. 그리고 다시 히트를 확인하여, 히트가 있는 캐릭터의 바디샷 카운트를 증가시키거나 새로운 바디샷 카운트를 시작합니다. 최종적으로 모든 히트박스를 원래 상태로 복구하고 결과를 반환합니다.
  
### CacheBoxPositions()
  이 메서드는 특정 캐릭터의 모든 히트 박스 위치, 회전 및 범위를 캐싱합니다. 이 정보는 FFramePackage 구조체에 저장되며, 나중에 지연 보상에 사용됩니다.

### MoveBoxes()
  이 메서드는 특정 캐릭터의 히트 박스들을 이전에 캐싱된 위치, 회전 및 범위로 이동시킵니다. 이 메서드는 서버가 과거 프레임으로 "되감기"할 때 사용됩니다.

### ResetHitBoxes() 
  이 메서드는 특정 캐릭터의 히트 박스들을 이전에 캐싱된 위치, 회전 및 범위로 이동시키고, 그 박스들의 충돌을 비활성화합니다. 이는 지연 보상 로직이 완료된 후 원래 상태로 돌아갈 때 사용됩니다.

### EnableCharacterMeshCollision() 
  이 메서드는 특정 캐릭터의 메시 충돌을 활성화 또는 비활성화합니다. 이것은 플레이어가 서버와 클라이언트 사이의 타임라인 차이로 인해 충돌을 놓칠 수 있는 상황을 방지하는 데 사용됩니다.

### ShowFramePackage() 
  이 메서드는 히트 박스들의 위치, 회전 및 범위를 디버그 렌더링으로 표시합니다. 이는 주로 개발 과정에서 지연 보상 로직의 정확성을 검사하는 데 사용됩니다.

### ServerSideRewind()
  이 메서드는 서버가 특정 시점으로 되감기하여 플레이어가 발사한 투사체가 특정 캐릭터를 맞췄는지 판정합니다.

### ProjectileServerSideRewind() 
  이 메서드는 서버가 특정 시점으로 되감기하여 플레이어가 발사한 투사체가 특정 캐릭터를 맞췄는지 판정합니다. 이 메서드는 투사체의 초기 속도를 고려하여 좀 더 복잡한 투사체 경로를 계산할 수 있습니다.

### ShotgunServerSideRewind() 
  이 메서드는 서버가 특정 시점으로 되감기하여 플레이어가 발사한 샷건 탄이 여러 캐릭터를 맞췄는지 판정합니다. 이 메서드는 여러 히트 위치를 고려하여 여러 캐릭터에 대한 히트 판정을 수행할 수 있습니다.
  
### GetFrameToCheck(ALetsShootCharacter HitCharacter, float HitTime): 
  이 메서드는 서버가 과거의 특정 시점(HitTime)으로 돌아가서 주어진 캐릭터(HitCharacter)의 상태를 검사하려 할 때 해당 시점의 캐릭터 상태를 찾습니다. 이 메서드는 특정 시점의 FFramePackage를 반환하며, 필요한 경우 두 시점 사이를 보간하여 결과를 생성합니다.

### ServerScoreRequest_Implementation(ALetsShootCharacter HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime): 
  이 메서드는 서버가 과거의 특정 시점(HitTime)으로 돌아가서 주어진 캐릭터(HitCharacter)가 특정 위치(HitLocation)에서 발사된 탄환에 맞았는지를 확인합니다. 지연 보상을 적용한 후, 실제로 캐릭터가 맞았다면 해당 캐릭터에 데미지를 적용합니다.

### ProjectileServerScoreRequest_Implementation(ALetsShootCharacter HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime): 
  이 메서드는 투사체의 서버 점수 요청을 처리합니다. 이 메서드는 서버가 과거의 특정 시점(HitTime)으로 돌아가서 주어진 캐릭터(HitCharacter)가 특정 위치(HitLocation)에서 발사된 투사체에 맞았는지를 확인합니다. 지연 보상을 적용한 후, 실제로 캐릭터가 맞았다면 해당 캐릭터에 데미지를 적용합니다.

### ShotgunServerScoreRequest_Implementation(const TArray<ALetsShootCharacter>& HitCharacters, const FVector_NetQuantize& TraceStart, const TArray<FVector_NetQuantize>& HitLocations, float HitTime): 
  이 메서드는 샷건의 서버 점수 요청을 처리합니다. 이 메서드는 서버가 과거의 특정 시점(HitTime)으로 돌아가서 주어진 캐릭터들(HitCharacters)이 특정 위치들(HitLocations)에서 발사된 샷건 탄에 맞았는지를 확인합니다. 지연 보상을 적용한 후, 실제로 캐릭터가 맞았다면 해당 캐릭터에 데미지를 적용합니다.

### TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction ThisTickFunction): 
  이 메서드는 각 틱마다 컴포넌트의 상태를 업데이트합니다. 이 경우에는 각 틱마다 새로운 프레임 패키지를 저장합니다.

### SaveFramePackage(): 
  이 메서드는 캐릭터의 현재 상태를 표현하는 프레임 패키지를 저장합니다. 이 메서드는 프레임 히스토리가 너무 오래되지 않도록 관리하며, 최신 상태를 항상 히스토리의 헤드에 추가합니다.

### SaveFramePackage(FFramePackage& Package): 
  이 메서드는 주어진 패키지(Package)에 캐릭터의 현재 상태를 저장합니다. 이 메서드는 캐릭터의 현재 위치, 회전, 박스 충돌 정보 등을 패키지에 저장합니다.

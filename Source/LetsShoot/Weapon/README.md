# Casing.h

### ACasing()

생성자에서 케이싱 메시 및 충돌, 물리 속성을 설정


### BeginPlay()

초기 실행 시, 메시 히트 이벤트에 OnHit 메서드 바인딩 및 탄피 추출 추진력 적용


### OnHit(UPrimitiveComponent*, AActor*, UPrimitiveComponent*, FVector, const FHitResult&)

케이싱이 다른 액터와 충돌할 때, 탄피 소리 재생 및 케이싱 파괴

---

# Flag.h

### AFlag()

생성자에서 메시 및 충돌 속성 설정


### Dropped()

깃발을 떨어뜨릴 때 실행되는 메서드로, 무기(깃발) 상태 설정 및 소유자 초기화


### ResetFlag()

깃발을 초기 상태로 리셋하는 메서드로, 깃발을 소유한 캐릭터 속성 변경 및 깃발 상태와 위치 초기화


### OnEquipped()

깃발이 장착되었을 때 실행되는 메서드로, HUD 위젯 숨기고 충돌 및 물리상태 설정


### OnDropped()

깃발이 떨어진 상태일 때 실행되는 메서드로, 충돌, 물리상태, 렌더링 업데이트 처리


### BeginPlay()

초기 실행 시, 초기 깃발 위치를 저장

---

# HitScanWeapon.h

### Fire(const FVector& HitTarget)

무기 발사를 처리하는 메서드로, 무기 가지고 있는 액터와 충돌 검사 후 데미지 적용, 파티클 및 소리 재생 처리


### WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit)

발사에 대한 충돌 검사하여 레이를 따라 타겟의 충돌을 확인하며, 빔 파티클 생성 및 충돌 결과 반환

---

# ProJectile.h

### AProjectile()

생성자에서 충돌 상자 및 물리 속성 설정


### BeginPlay()

초기 실행 시, 트레이서 생성 및 'OnHit' 이벤트 바인딩


### OnHit()

프로젝타일이 목표물에 충돌했을 때 실행되는 메서드로, 프로젝타일 파괴 처리


### SpawnTrailSystem()

특정 효과 시스템을 통해 프로젝타일 꼬리 생성


### ExplodeDamage()

프로젝타일 이펙트 반경 내 액터에 대한 데미지와 피해의 범위 적용


### Tick(float DeltaTime)

프로젝타일의 위치 및 회전 갱신 처리


### StartDestroyTimer()

프로젝타일이 파괴될 때까지의 시간을 관리하는 타이머 시작


### DestroyTimerFinished()

지정된 시간 후 프로젝타일 파괴 처리


### Destroyed()

파괴된 프로젝타일에 대한 충돌 이펙트 및 사운드 재생 처리

---

# ProjectileBullet.h

### AProjectileBullet::AProjectileBullet() : 
생성자 메서드로, 프로젝타일 발사체의 기본 속성을 초기화하는 데 사용됩니다. 여기서 발사체 움직임 컴포넌트가 생성되고 필요한 속성 값이 설정됩니다.


### #if WITH_EDITOR void AProjectileBullet::PostEditChangeProperty(FPropertyChangedEvent& Event) #endif: 
에디터에서 속성 변경 이벤트 처리 메서드로, InitialSpeed 속성 값이 변경될 때 발사체 움직임 컴포넌트의 속도도 동기화됩니다.



### void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) : 
발사체가 목표물에 충돌한 경우 호출되는 메서드로, 소유한 플레이어 컨트롤러와 충돌 대상을 확인하여 데미지를 입힙니다. 서버 사이드 리와인드를 사용하면, 서버에 요청사항을 전달합니다.

---

# ProjectileGrenade.h

### AProjectileGrenade::AProjectileGrenade() : 
생성자 메서드로, 수류탄 발사체의 기본 속성을 초기화하는 데 사용됩니다. 발사체 메시와 발사체 움직임 컴포넌트를 생성하며, 움직임 컴포넌트의 속성을 설정한다.


### void AProjectileGrenade::BeginPlay() : 
발사체가 게임 내에서 생성될 때 호출되는 메서드로, 발사체의 트레일 시스템을 생성하고 파괴 타이머를 시작합니다. 발사체 움직임 컴포넌트의 OnProjectileBounce 이벤트에 OnBounce 메서드를 바인딩합니다.


### void AProjectileGrenade::OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity) : 
발사체가 튕겼을 때 호출되는 메서드로, BounceSound가 설정되어 있으면 튕김 소리를 재생합니다.


### void AProjectileGrenade::Destroyed() : 
발사체가 파괴될 때 호출되는 메서드로, 발사체가 폭발하면 데미지를 처리한 후 상위 클래스의 Destroyed() 메서드를 호출합니다.

---

# ProjectileRocket.h

### AProjectileRocket::AProjectileRocket() : 
생성자 메서드로, 로켓 발사체의 기본 속성을 초기화하는 데 사용됩니다. 발사체 메시와 RocketMovementComponent를 생성하며, 움직임 컴포넌트의 속성을 설정합니다.


### void AProjectileRocket::BeginPlay() : 
발사체가 게임 내에서 생성될 때 호출되는 메서드로, 충돌 이벤트를 처리하고 트레일 시스템을 생성합니다. 루프 사운드가 설정되어 있다면, 이를 재생합니다.


### void AProjectileRocket::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) : 
발사체가 목표물에 충돌한 경우 호출되는 메서드로, 데미지를 처리하고 필요한 시각 및 음향 효과를 처리한 후 발사체를 파괴할 데미지 타이머를 시작합니다.


### void AProjectileRocket::Destroyed() : 
발사체가 파괴될 때 호출되는 메서드입니다. 추가적인 작업을 할 수 있으나, 현재는 빈 메서드로 냅둡니다.

---

# ProjectileWeapon.h

### AProjectileWeapon::Fire(const FVector& HitTarget) : 
무기가 발사되는 메서드로, 발사체를 생성하고 적절한 설정을 적용합니다.

APawn* InstigatorPawn = Cast<APawn>(GetOwner()); 무기 소유자(플레이어)를 가져옵니다.

머즐 플래시 소켓의 위치와 회전각을 구하는 데 사용합니다.

프로젝타일의 대상 위치와 회전을 계산합니다.

서버와 클라이언트에서 발사체를 생성하고 적절한 서버 사이드 리와인드(SSR) 및 데미지 설정을 적용합니다.


이 메서드는 서버와 클라이언트 모두에서 효율적으로 작동하도록 구성되어 있어, 실행 환경에 따른 다양한 발사체 설정이 적용됩니다.
  
---
  
# RocketMovementComponent.h
  
### URocketMovementComponent::HandleBlockingHit(const FHitResult& Hit, float TimeTick, const FVector& MoveDelta, float& SubTickTimeRemaining) : 
로켓이 충돌한 경우 처리하는 메서드입니다. 상위 클래스의 HandleBlockingHit을 호출한 후 AdvanceNextSubstep으로 결과를 반환하여 다음 단계로 진행합니다.


### URocketMovementComponent::HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta) : 
로켓이 충격을 받는 경우 처리하는 메서드입니다. 로켓이 중지되지 않도록 이 메서드는 비어있으며, 로켓은 CollisionBox가 충돌 감지할 때만 폭발합니다.

---
  
# ShotGun.h
  
### AShotgun::FireShotgun(const TArray<FVector_NetQuantize>& HitTargets) : 
샷건 발사를 처리하는 메서드입니다. 샷건의 각 팔릿에 대해 충돌을 감지하고, 충돌한 캐릭터들에게 상위 데미지가 적용되도록 계산해서 처리합니다.


### AShotgun::ShotgunTraceEndWithScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets) : 
샷건의 팔릿이 퍼지는 형태로 발사되는 경로를 계산하는 메서드입니다. 지정된 거리에서, SphereRadius 범위 내에서 무작위 방향으로 팔릿의 종단점을 생성하여 여러 각도로 퍼지는 샷건 효과를 만듭니다.
  
---
  
# Weapon.h
  
### AWeapon::AWeapon():
생성자에서 무기 초기화를 처리합니다. 컴포넌트를 추가하고 설정합니다.


### EnableCustomDepth(bool bEnable):
무기의 커스텀 깊이 랜더링을 사용할 것인지 설정합니다.


### BeginPlay():
게임 시작 시 실행되어 무기의 오버랩 및 충돌 이벤트를 설정합니다.


### Tick(float DeltaTime):
무기의 Tick 함수입니다. 현재 비어있습니다.


### GetLifetimeReplicatedProps(): 
무기의 네트워크 속성을 설정합니다.


### OnSphereOverlap(), OnSphereEndOverlap(): 
무기와 다른 캐릭터와의 오버랩 이벤트 처리입니다. 오버랩 중인지 여부를 기록합니다.


### SetHUDAmmo(): 
현재 무기의 탄약을 HUD에 표시합니다.


### SpendRound(): 
무기의 탄약을 소비하고 이를 서버와 동기화합니다.


### ClientUpdateAmmo_Implementation(): 
클라이언트가 탄약 변경 사항을 동기화하는 메서드입니다.


### AddAmmo(), ClientAddAmmo_Implementation(): 
무기의 탄약을 추가하고 이를 서버와 동기화합니다.


### OnRep_Owner(): 
무기의 소유자가 변경될 때 호출되어 상태를 설정합니다.


### SetWeaponState(), OnWeaponStateSet(): 
무기의 상태를 설정하고 무기의 상태에 따라 특정 기능을 실행합니다.


### OnPingTooHigh(): 
핑이 높은 경우 서버 측 재생도 사용하지 않도록 설정합니다.


### OnRep_WeaponState(): 
네트워크에서 무기의 상태가 동기화되면 호출되어 상태 처리 기능을 실행합니다.
  
### OnEquipped(): 
무기를 장착했을 때 호출되는 함수입니다. 픽업 위젯을 숨기고, 무기의 충돌, 중력 및 물리 시뮬레이션을 비활성화합니다. 서브머신건의 경우 특정 설정을 적용하며 필요에 따라 HighPingDelegate에 이벤트를 바인드합니다.
  
#### 서브머신건의 경우...
서브머신건은 다른 무기와 비교하여 몇 가지 특징이 있습니다. 이러한 특징들로 인해 서브 머신건에 관련된 설정을 별도로 적용해야 하는 경우가 있습니다. 서브머신건의 주요 특징은 다음과 같습니다.
고사속도: 서브머신건은 일반적으로 총알이 빠르게 연속 발사되는 고사속도 무기입니다. 이로 인해 게임에서는 발사 사이의 시간 간격을 짧게 설정해야 합니다.
탄피 제거: 서브머신건과 같은 자동 소총류의 경우, 총알을 발사할 때마다 탄피가 배출됩니다. 게임 내에서는 이러한 물리적 이펙트를 구현해야 하며, 그에 따른 부하를 관리할 필요가 있습니다.
반동과 정확도: 서브머신건은 높은 발사 속도 때문에 반동이 심한 편입니다. 게임에서는 이를 고려하여 무기의 반동 효과와 정확도를 조절할 수 있는 매커니즘이 필요합니다.
고속 발사로 인한 네트워크 영향: 서브머신건은 고사속도 무기로 인해 발사 이벤트가 자주 발생합니다. 온라인 게임에서는 이로 인해 고핑이 발생할 수 있으며, 이에 따라 게임 내에서 적절한 네트워크 대응이 필요합니다.
이러한 이유로 서브머신건의 경우 특정 설정이 필요합니다. 이러한 설정은 게임 물리 엔진, 애니메이션, 네트워크 최적화 등에 영향을 주게 됩니다.


### OnDropped(): 
무기를 드롭했을 때 호출되는 함수입니다. 충돌 설정을 변경하고, 무기의 프로퍼티 및 위젯을 업데이트합니다. HighPingDelegate 이벤트를 언바인드합니다.


### OnEquippedSecondary(): 
무기를 보조 무기로 장착했을 때 호출되는 함수입니다. 로직은 OnEquipped()와 유사하지만 CustomDepthStencilValue를 다르게 설정합니다.


### ShowPickupWidget(bool bShowWidget): 
픽업 위젯의 가시성을 설정하는 함수입니다.


### Fire(const FVector& HitTarget): 
무기를 발사하는 함수로 발사 애니메이션을 재생하고 탄피를 생성합니다. 한 발을 소모합니다.


### Dropped(): 
무기의 상태를 드롭 상태로 변경하고, 무기의 컴포넌트를 해제하며 소유주 정보를 초기화합니다.


### IsEmpty(): 
무기의 탄약이 비어 있는지 확인하는 함수입니다.


### IsFull(): 
무기의 탄약이 가득 찼는지 확인하는 함수입니다.


### TraceEndWithScatter(const FVector& HitTarget): 
발사 시 퍼지는 트레이스 끝 지점을 계산하는 함수입니다.
  
---

# WeaponTypes.h
  
이 코드는 무기 유형을 정의하는 열거형(enum)과 무기 관련 상수를 포함하고 있습니다. 각 요소는 다음과 같습니다.

### TRACE_LENGTH: 
트레이스의 최대 길이를 설정하는 상수입니다. 이 값은 무기가 적용할 충돌 검출 로직과 관련이 있습니다.


### CUSTOM_DEPTH_PURPLE, CUSTOM_DEPTH_BLUE, CUSTOM_DEPTH_TAN: 
각 무기의 사용자 정의 깊이 값을 설정하는 상수입니다. 이 값은 무기와 다른 객체간의 비주얼 레이어를 구분하는 데 사용됩니다.
#### 사용자 정의 깊이 값(Custom Depth)
사용자 정의 깊이 값(Custom Depth)은 렌더링 파이프라인에서 객체 간의 Z-순서(깊이)를 구분하는 데 사용되는 값입니다. 이 값은 렌더링 시각화를 위해 대상 오브젝트와 카메라 사이의 거리를 결정하는 방법을 제공합니다.
비주얼 레이어를 구분하는 것은 게임 내 객체들이 다른 객체들에 덮이지 않고, 올바르게 나타나도록 순서를 조절하는 프로세스를 의미합니다. 사용자 정의 깊이 값을 사용하면 레이어 간의 가시성을 제어하고, 겹치는 렌더링 문제를 해결할 수 있습니다.
예를 들어, CUSTOM_DEPTH_PURPLE, CUSTOM_DEPTH_BLUE, CUSTOM_DEPTH_TAN 상수를 사용하여 무기를 서로 다른 비주얼 레이어에 배치할 수 있고, 이를 통해 화면에 무기가 올바르게 표시되도록 구현할 수 있습니다. 이러한 방식으로 객체들이 함께 그려질 때 겹치지 않도록 설정할 수 있습니다.


### EWeaponType: 
무기 유형을 나타내는 열거형(enum)입니다. 다양한 종류의 무기를 정의하고 있으며, 각각은 각기 다른 동작과 속성을 가질 수 있습니다.


### EWT_AssaultRifle: 
돌격 소총 무기 유형입니다.

### EWT_RocketLauncher: 
로켓 발사기 무기 유형입니다.

### EWT_Pistol: 
권총 무기 유형입니다.

### EWT_SubmachineGun: 
서브머신건 무기 유형입니다.

### EWT_Shotgun: 
샷건 무기 유형입니다.

### EWT_SniperRifle: 
저격소총 무기 유형입니다.

### EWT_GrenadeLauncher: 
수류탄 발사기 무기 유형입니다.

### EWT_Flag: 
깃발 무기 유형입니다. 

### EWT_MAX: 
기본적인 최댓값입니다. 이 값은 배열 초기화 등에서 사용될 수 있습니다.

# GetAmmo.h

### AGetAmmo::OnSphereOverlap(...)

오버랩 이벤트 발생 시 호출되는 메서드

오버랩 대상을 처리하고, 캐릭터의 전투 컴포넌트를 참조하여 탄약을 주움

처리가 완료되면 오브젝트를 파괴하여 화면에서 삭제

---

# GetHealth.h

### AGetHealth()

생성자에서 bReplicates 값을 true로 설정


### OnSphereOverlap(UPrimitiveComponent, AActor, UPrimitiveComponent, int32, bool, const FHitResult)

픽업과 플레이어가 겹칠 때 실행됨

ALetsShootCharacter 객체 플레이어를 통해 Buff 컴포넌트에 접근

플레이어의 Buff 컴포넌트로부터 Heal() 함수 호출하여 HealAmount 액체와 HealingTime 액체로 회복

픽업 오브젝트 파괴

---

# GetJump.h

### OnSphereOverlap(UPrimitiveComponent, AActor, UPrimitiveComponent, int32, bool, const FHitResult)

픽업과 플레이어가 겹칠 때 실행되는 메서드

ALetsShootCharacter 객체 플레이어를 통해 Buff 컴포넌트를 획득

플레이어의 Buff 컴포넌트로부터 BuffJump() 함수를 호출하여 JumpZVelocityBuff 값으로 점프 높이가 증가하고 JumpBuffTime 동안 유지

픽업 오브젝트 파괴


---

# Item.h

### AItem()

생성자에서 컴포넌트 초기화 및 구성 수행


### BeginPlay()

실행 시점 초기화에 대한 것들 처리한다. 이 경우, 오버랩 이벤트 바인딩에 관련된 타이머 설정


### OnSphereOverlap(UPrimitiveComponent*, AActor*, UPrimitiveComponent*, int32, bool, const FHitResult&)

오버랩 이벤트 발생 시 호출되는 메서드


### BindOverlapTimerFinished()

BindOverlapTime 이후에 OnSphereOverlap 메서드를 오버랩 이벤트에 바인딩


### Tick(float)

프레임에 따라 호출되는 메서드로 픽업 메시의 회전 수행


### Destroyed()

픽업 오브젝트가 파괴될 때 호출되는 메서드로, 사운드와 이펙트 재생 처리

---

# ItemSpawnPoint.h

### AItemSpawnPoint()

생성자에서 타이머 및 복제 관련 설정 수행


### BeginPlay()

초기 실행 시, 스폰 지점에서 픽업 오브젝트를 생성하기 위한 타이머 시작


### SpawnItem()

설정된 픽업 클래스 중 무작위로 하나의 픽업 오브젝트 생성

생성 시, 오브젝트가 파괴되면 픽업 타이머 시작 이벤트 바인딩


### SpawnItemTimerFinished()

스폰 픽업 타이머 종료 시 호출되어 픽업 오브젝트 생성


### StartSpawnItemTimer(AActor*)

주어진 대기 시간 후에 픽업 오브젝트가 스폰되도록 타이머 설정


### Tick(float)

본 프로젝트 문맥에서 사용되지 않는 메서드, Tick이 off되어 있음

---

# GetShield.h

### OnSphereOverlap(UPrimitiveComponent*, AActor*, UPrimitiveComponent*, int32, bool, const FHitResult&)

픽업과 플레이어가 겹칠 때 실행되는 메서드

ALetsShootCharacter 객체를 통해 플레이어의 Buff 컴포넌트를 획득

플레이어의 Buff 컴포넌트로부터 ReplenishShield() 함수를 호출하여 ShieldReplenishAmount 만큼 Shield를 보충하고, ShieldReplenishTime 동안 지속됨

픽업 오브젝트 파괴

---

# GetSpeed.h

### OnSphereOverlap(UPrimitiveComponent*, AActor*, UPrimitiveComponent*, int32, bool, const FHitResult&)

스피드 픽업과 플레이어가 겹칠 때 실행되는 메서드

ALetsShootCharacter 객체를 통해 플레이어의 Buff 컴포넌트를 획득

플레이어의 Buff 컴포넌트를 통해 BuffSpeed() 함수를 호출하여 속도를 증가시킴

일반 속도 : BaseSpeedBuff 적용

앉은 속도 : CrouchSpeedBuff 적용

버프 지속 시간 : SpeedBuffTime 동안 지속됨

픽업 오브젝트 파괴


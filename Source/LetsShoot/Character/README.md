# LetsShootAnimInstance.h

이 코드는 LetsShootAnimInstance 클래스의 메서드를 정의하며, 애니메이션에 필요한 변수를 초기화하고 업데이트하는 역할을 합니다.

### NativeInitializeAnimation(): 
애니메이션 초기화 메서드로, 초기화 작업을 수행합니다. LetsShootCharacter를 애니메이션 소유자의 Pawn(캐릭터)로 캐스팅하여 할당합니다.

### NativeUpdateAnimation(float DeltaTime): 
애니메이션 업데이트 메서드로, 각 프레임마다 DeltaTime 값과 함께 호출됩니다. 이 메서드에서 LetsShootCharacter의 속성 값을 애니메이션 속성에 할당하고, 필요한 계산을 수행한 후 알맞은 애니메이션을 실행할 수 있도록 값들을 설정합니다.

속도, 공중에 있는지 여부, 가속 중인지 여부, 무기 장착 여부, 외부에서 영향 받는 요소 계산 등

왼손 위치, 회전 처리, 목표 지점을 향하는 시선 처리 등 무기와 관련된 애니메이션 변수 처리

FABRIK, AimOffsets, RightHand 변환 사용 여부 결정 등 기타 규칙 기반 애니메이션 값 설정


이 클래스는 애니메이션 블루프린트에서 사용할 변수들을 관리하고, 캐릭터의 상태에 따라 애니메이션 블루프린트에서 알맞은 애니메이션 구성 요소를 선택하고 실행할 수 있는 정보를 제공합니다. 이로써 LetsShootCharacter의 애니메이션이 정확하게 동작할 수 있습니다.

---

# LetsShootCharacter.h

이 코드는 ALetsShootCharacter 클래스의 생성자 및 메서드를 정의하며, 캐릭터의 초기화 및 다양한 기능을 구현합니다.

### ALetsShootCharacter(): 

생성자로 클래스의 초기 설정을 수행합니다.

카메라, 위젯, 충돌 박스 등 필요한 컴포넌트를 생성 및 설정

속성 값들을 초기화하고 설정


### GetLifetimeReplicatedProps(): 

캐릭터 클래스의 네트워크 저장된 속성들을 설정합니다.

OverlappingWeapon, Health, Shield, bDisableGameplay 등 복제해야 할 속성 설정


### OnRep_ReplicatedMovement(): 

객체의 복제된 움직임에 응답하여 호출되는 함수입니다.

복제된 움직임의 시뮬레이션 작업을 수행합니다.


### Elim(bool bPlayerLeftGame): 

캐릭터가 사망하거나 게임에서 나갈 경우 호출되는 함수입니다.

캐릭터를 소멸시키기 전 무기를 드롭하거나 소멸시킵니다.


### MulticastElim_Implementation(bool bPlayerLeftGame): 

Elim 함수에 의해 호출되며, 사망 애니메이션 및 이펙트, 사운드를 재생합니다.

사망후 관련 사운드, 분해 효과, 컴포넌트 제거 등

회전률, 이동, 충돌 등을 비활성화하여 게임 상에서 제거 처리

### ElimTimerFinished(): 

이 메서드는 캐릭터의 일정 시간 후에 부활을 요청하거나 게임에서 나간 경우 이벤트를 방송합니다.


### ServerLeaveGame_Implementation(): 

서버에서 실행되며, 캐릭터가 게임을 떠날 때 해당 플레이어의 상태를 게임 모드의 PlayerLeftGame 메서드로 전달합니다.


### DropOrDestroyWeapon(): 

무기를 파괴하거나 드롭하는 로직을 수행합니다.


### DropOrDestroyWeapons(): 

전투용 캐릭터가 가지고 있는 무기들을 모두 드롭하거나 파괴합니다.


### OnPlayerStateInitialized(): 

플레이어의 상태가 초기화되면 호출되는 메서드로, 팀 색상을 설정하고 스폰 포인트를 설정합니다.


### SetSpawnPoint(): 

캐릭터가 스폰 될 위치를 설정합니다.


### Destroyed(): 

캐릭터가 파괴되었을 때 호출되며, 이벤트 처리와 무기 등 리소스 제거 작업을 수행합니다.


### MulticastGainedTheLead_Implementation() & MulticastLostTheLead_Implementation(): 

캐릭터가 선두를 기록하거나 잃었을 때 이벤트를 처리하는 메서드입니다.


### SetTeamColor(): 

팀 색상을 설정합니다.


### BeginPlay() & Tick(): 

게임 시작 시 호출됩니다. 데미지 처리 이벤트 등록 등 다양한 초기화 작업을 실행하고 주기적으로 캐릭터의 움직임을 처리합니다.


### RotateInPlace(): 

캐릭터의 회전 처리를 수행합니다.


### SetupPlayerInputComponent(): 

플레이어의 입력 처리를 설정합니다.


### PostInitializeComponents(): 

컴포넌트 초기화 후 호출되며, 컴포넌트 간 참조 및 초기 설정 작업을 수행합니다.


### PlayFireMontage(bool bAiming): 

캐릭터 발사 애니메이션을 재생, bAiming이 참이면 조준 발사, 그렇지 않으면 힙 발사 애니메이션을 재생.


### PlayReloadMontage(): 

캐릭터 장전 애니메이션을 재생. 무기의 종류에 따라서 해당 애니메이션 섹션으로 점프.




### PlayElimMontage(): 

캐릭터의 처치 애니메이션을 재생.


### PlayThrowGrenadeMontage(): 

캐릭터의 수류탄 던지기 애니메이션을 재생.


### PlaySwapMontage():

무기 교체 애니메이션을 재생.


### PlayHitReactMontage(): 

캐릭터가 피격 후 피격 리액션 애니메이션을 재생.


### GrenadeButtonPressed(): 

수류탄을 던지기 위해 눌렀을 때 호출되는 메서드. Combat의 ThrowGrenade() 호출.


### ReceiveDamage(): 

캐릭터에 피해를 입히는 메서드. 피해를 처리하고, HUD 업데이트, 피격 리액션 애니메이션 등 수행.


### MoveForward(float Value): 

앞/뒤 이동 메서드. 방향 벡터를 계산하여 입력에 따른 이동 처리.


### MoveRight(float Value): 

오른쪽/왼쪽 이동 메서드. 방향 벡터를 계산하여 입력에 따른 이동 처리.


### Turn(float Value): 

캐릭터 회전 메서드, Value에 따라 좌/우 회전 처리.


### LookUp(float Value): 

캐릭터 상/하 눈길 메서드, Value에 따라 상/하 처리.


### EquipButtonPressed(): 

무기 장착 버튼을 누를 때의 메서드. 서버에서 장비 처리 및 무기 교체 애니메이션이 재생.


### ServerEquipButtonPressed_Implementation(): 

서버에서 무기 장착 처리를 담당.


### CrouchButtonPressed(): 

앉기 버튼을 누를 때 앉기/일어서기 상태 전환 처리.

### ReloadButtonPressed(): 

재장전 버튼이 눌렸을 때 호출되는 메서드로, 게임 플레이가 비활성화되거나 깃발을 들고 있지 않을 때만 재장전이 가능합니다.


### AimButtonPressed(): 

조준 버튼이 눌렸을 때 호출되는 메서드로, 플레이어는 깃발을 들고 있지 않고 게임 플레이가 활성화된 상태에서만 조준할 수 있습니다.


### AimButtonReleased(): 

조준 버튼이 눌리지 않았을 때 호출되는 메서드로, 캐릭터가 조준을 중단합니다.


### CalculateSpeed(): 

캐릭터의 속도를 계산하는 메서드입니다.


### AimOffset(): 

조준 오프셋을 계산하는 메서드로, 캐릭터가 움직이거나 공중에 있을 때와 아닌 경우를 구분하여 조준 처리를 수행합니다.


### CalculateAO_Pitch(): 

조준 오프셋(AO)의 피치 값을 계산하는 메서드입니다.


### SimProxiesTurn(): 

서버와 동기화된 클라이언트의 회전을 처리하는 메서드입니다.


### Jump(): 

캐릭터가 점프하는 메서드로, 게임 플레이가 활성화되고 깃발을 들고 있지 않을 때 호출됩니다.


### FireButtonPressed(): 

발사 버튼이 눌렸을 때 호출되는 메서드로, 게임 플레이가 활성화되고 깃발을 들고 있지 않을 때 발사 가능합니다.


### FireButtonReleased(): 

발사 버튼이 떼어졌을 때 호출되는 메서드로, 캐릭터가 총을 쏘는 것을 중단합니다.


### TurnInPlace(): 

캐릭터가 제자리에서 회전하는 것을 처리하는 메서드입니다. 시간 변화에 따라 적절한 회전 애니메이션을 수행합니다.

### HideCameraIfCharacterClose(): 

카메라와 캐릭터 간의 거리가 일정 거리 이내일 때 캐릭터와 무기 메시를 숨기는 메서드입니다.


### OnRep_Health(): 

플레이어의 Health 값이 변경될 때, 피격 반응 애니메이션을 재생하고 HUD health를 업데이트합니다.


### OnRep_Shield(): 

플레이어의 Shield 값이 변경될 때, 피격 반응 애니메이션을 재생하고 HUD shield를 업데이트합니다.


### UpdateHUDHealth(): 

HUD의 Health 값을 업데이트하는 메서드입니다.


### UpdateHUDShield(): 

HUD의 Shield 값을 업데이트하는 메서드입니다.


### UpdateHUDAmmo(): 

HUD의 탄약량을 업데이트하는 메서드입니다.


### SpawDefaultWeapon(): 

캐릭터가 게임 시작 시 기본 무기를 스폰해주는 메서드입니다.


### PollInit(): 

PlayerState 및 PlayerController 초기화상태를 폴링하고, 필요한 경우 스폰, HUD 업데이트, 초기화 작업을 수행합니다.


### UpdateDissolveMaterial(): 

머티리얼의 디졸브 값을 업데이트하는 메서드입니다.


### StartDissolve(): 

디졸브 애니메이션을 실행하는 메서드입니다.


### SetOverlappingWeapon(): 

겹치는 무기를 설정하고, 픽업 위젯을 표시하는 메서드입니다.


### OnRep_OverlappingWeapon(): 

레플리케이션 작업을 처리하고 픽업 위젯 표시를 업데이트하는 메서드입니다.


### IsWeaponEquipped(): 

무기가 장착되었는지 확인하는 메서드입니다.


### IsAiming(): 

캐릭터가 조준 중인지 확인하는 메서드입니다.


### GetEquippedWeapon(): 

현재 장착된 무기를 반환하는 메서드입니다.


### GetHitTarget(): 

피격 대상 위치를 반환하는 메서드입니다.


### GetCombatState(): 

캐릭터의 현재 전투 상태를 반환하는 메서드입니다.


### IsLocallyReloading(): 

캐릭터가 로컬에서 재장전 중인지 확인하는 메서드입니다.


### IsHoldingTheFlag(): 

캐릭터가 깃발을 들고 있는지 확인하는 메서드입니다.


### GetTeam(): 

플레이어의 팀을 가져오는 메서드입니다.


### SetHoldingTheFlag(): 

깃발을 들고 있는 상태를 설정하는 메서드입니다.

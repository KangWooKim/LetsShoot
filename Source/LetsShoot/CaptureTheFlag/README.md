# FlagZone.h

### AFlagZone::AFlagZone()

이 메서드는 AFlagZone 클래스의 생성자입니다. 객체가 생성될 때 이 메서드가 호출되며, 이 클래스의 변수들을 초기화합니다. 여기서는 PrimaryActorTick.bCanEverTick 변수를 false로 설정하여 이 Actor가 Tick 함수를 호출하지 않도록 설정하고 있습니다. 그리고 ZoneSphere 객체를 생성하여 이를 루트 컴포넌트로 설정합니다. 이 ZoneSphere는 플래그 존의 영역을 정의하는 구체 형태의 컴포넌트입니다.

### void AFlagZone::BeginPlay()

이 메서드는 Actor가 게임 플레이를 시작할 때 호출됩니다. 이 메서드에서는 ZoneSphere의 충돌 시작 이벤트에 OnSphereOverlap 메서드를 바인딩하여, 다른 Actor가 이 구체 영역에 들어왔을 때 OnSphereOverlap 함수가 호출되도록 설정하고 있습니다.

### void AFlagZone::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)

이 메서드는 ZoneSphere와 다른 Actor가 겹쳤을 때 호출되는 함수입니다. 이 함수 내부에서는 겹친 Actor가 AFlag 클래스의 인스턴스인지 확인하고, 그렇다면 그 플래그의 팀이 이 FlagZone의 팀과 다른지 확인합니다. 플래그의 팀이 이 FlagZone의 팀과 다르다면 (즉, 상대 팀의 플래그가 이 플래그 존에 들어왔다면), FlagCaptured 함수를 호출하여 플래그가 점령되었음을 알리고, 플래그를 리셋하는 ResetFlag 함수를 호출합니다. 이를 통해 플래그는 원래의 위치로 돌아갑니다. 이 과정을 통해 플래그 점령이라는 게임 로직을 구현하고 있습니다.

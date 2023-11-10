
#pragma once // 중복 포함을 방지하기 위해 프래그마 원스를 사용합니다.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h" 

// 멀티플레이어 세션 모듈을 위한 클래스 선언입니다. IModuleInterface를 상속받아 모듈의 생명 주기를 관리합니다.
class FMultiplayerSessionsModule : public IModuleInterface
{
public:

    // 모듈이 시작될 때 호출되는 메서드입니다. 리소스 할당 등의 초기화 작업을 수행합니다.
    virtual void StartupModule() override;

    // 모듈이 종료될 때 호출되는 메서드입니다. 할당된 리소스의 해제 등의 정리 작업을 수행합니다.
    virtual void ShutdownModule() override;
};

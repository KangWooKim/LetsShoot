// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerSessions.h"

#define LOCTEXT_NAMESPACE "FMultiplayerSessionsModule"

// FMultiplayerSessionsModule 클래스의 StartupModule 메서드 구현부입니다.
void FMultiplayerSessionsModule::StartupModule()
{
    // 이 코드는 모듈이 메모리에 로드된 후 실행됩니다;
    // 모듈이 시작될 때 필요한 리소스 할당, 초기 설정, 이벤트 구독 등의 초기화 작업을 여기서 수행합니다.
}

// FMultiplayerSessionsModule 클래스의 ShutdownModule 메서드 구현부입니다.
void FMultiplayerSessionsModule::ShutdownModule()
{
    // 이 함수는 모듈이 종료되는 동안 호출될 수 있습니다. 동적 리로딩을 지원하는 모듈의 경우,
    // 모듈을 언로딩하기 전에 이 함수를 호출합니다.
    // 여기서는 할당된 리소스 해제, 이벤트 구독 해제 등 모듈 종료 시 필요한 정리 작업을 수행합니다.
}

// LOCTEXT_NAMESPACE 매크로 정의를 해제합니다. 이것은 이전에 정의된 텍스트 네임스페이스를 '언디파인' 하여 충돌을 방지합니다.
#undef LOCTEXT_NAMESPACE

// FMultiplayerSessionsModule 클래스를 Unreal Engine의 모듈 시스템에 구현(등록)합니다. 
// 이것은 엔진이 모듈의 존재를 인식하고, StartupModule과 ShutdownModule을 적절한 시점에 호출하게 만듭니다.
IMPLEMENT_MODULE(FMultiplayerSessionsModule, MultiplayerSessions)

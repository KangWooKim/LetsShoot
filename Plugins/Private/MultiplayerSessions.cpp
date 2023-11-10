// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultiplayerSessions.h"

#define LOCTEXT_NAMESPACE "FMultiplayerSessionsModule"

// FMultiplayerSessionsModule Ŭ������ StartupModule �޼��� �������Դϴ�.
void FMultiplayerSessionsModule::StartupModule()
{
    // �� �ڵ�� ����� �޸𸮿� �ε�� �� ����˴ϴ�;
    // ����� ���۵� �� �ʿ��� ���ҽ� �Ҵ�, �ʱ� ����, �̺�Ʈ ���� ���� �ʱ�ȭ �۾��� ���⼭ �����մϴ�.
}

// FMultiplayerSessionsModule Ŭ������ ShutdownModule �޼��� �������Դϴ�.
void FMultiplayerSessionsModule::ShutdownModule()
{
    // �� �Լ��� ����� ����Ǵ� ���� ȣ��� �� �ֽ��ϴ�. ���� ���ε��� �����ϴ� ����� ���,
    // ����� ��ε��ϱ� ���� �� �Լ��� ȣ���մϴ�.
    // ���⼭�� �Ҵ�� ���ҽ� ����, �̺�Ʈ ���� ���� �� ��� ���� �� �ʿ��� ���� �۾��� �����մϴ�.
}

// LOCTEXT_NAMESPACE ��ũ�� ���Ǹ� �����մϴ�. �̰��� ������ ���ǵ� �ؽ�Ʈ ���ӽ����̽��� '�������' �Ͽ� �浹�� �����մϴ�.
#undef LOCTEXT_NAMESPACE

// FMultiplayerSessionsModule Ŭ������ Unreal Engine�� ��� �ý��ۿ� ����(���)�մϴ�. 
// �̰��� ������ ����� ���縦 �ν��ϰ�, StartupModule�� ShutdownModule�� ������ ������ ȣ���ϰ� ����ϴ�.
IMPLEMENT_MODULE(FMultiplayerSessionsModule, MultiplayerSessions)

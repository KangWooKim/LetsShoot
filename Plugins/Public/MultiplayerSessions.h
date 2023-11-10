
#pragma once // �ߺ� ������ �����ϱ� ���� �����׸� ������ ����մϴ�.

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h" 

// ��Ƽ�÷��̾� ���� ����� ���� Ŭ���� �����Դϴ�. IModuleInterface�� ��ӹ޾� ����� ���� �ֱ⸦ �����մϴ�.
class FMultiplayerSessionsModule : public IModuleInterface
{
public:

    // ����� ���۵� �� ȣ��Ǵ� �޼����Դϴ�. ���ҽ� �Ҵ� ���� �ʱ�ȭ �۾��� �����մϴ�.
    virtual void StartupModule() override;

    // ����� ����� �� ȣ��Ǵ� �޼����Դϴ�. �Ҵ�� ���ҽ��� ���� ���� ���� �۾��� �����մϴ�.
    virtual void ShutdownModule() override;
};

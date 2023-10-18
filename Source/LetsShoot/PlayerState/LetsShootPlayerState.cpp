// Fill out your copyright notice in the Description page of Project Settings.


#include "LetsShootPlayerState.h"
#include "LetsShoot/Character/LetsShootCharacter.h"
#include "LetsShoot/PlayerController/LetsShootPlayerController.h"
#include "Net/UnrealNetwork.h"

// �÷��̾� ���¸� ������ �Ӽ� ����
void ALetsShootPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
    // ���� Ŭ������ �Ӽ��� ����
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    // �� �� �Ӽ��� ��Ʈ��ũ�� ���� �����Ǿ�� ��
    DOREPLIFETIME(ALetsShootPlayerState, Defeats);
    DOREPLIFETIME(ALetsShootPlayerState, Team);
}

// �÷��̾� ���� ����
void ALetsShootPlayerState::AddToScore(float ScoreAmount)
{
    // �� ������ ScoreAmount�� �߰�
    SetScore(GetScore() + ScoreAmount);
    // ĳ���͸� ��� HUD�� ������ ����
    Character = Character == nullptr ? Cast<ALetsShootCharacter>(GetPawn()) : Character;
    if (Character)
    {
        Controller = Controller == nullptr ? Cast<ALetsShootPlayerController>(Character->Controller) : Controller;
        if (Controller)
        {
            Controller->SetHUDScore(GetScore());
        }
    }
}

// ������ ����� �� HUD�� ������Ʈ
void ALetsShootPlayerState::OnRep_Score()
{
    // ���� Ŭ������ �Լ� ȣ��
    Super::OnRep_Score();

    // ĳ���͸� ��� HUD�� ������ ����
    Character = Character == nullptr ? Cast<ALetsShootCharacter>(GetPawn()) : Character;
    if (Character)
    {
        Controller = Controller == nullptr ? Cast<ALetsShootPlayerController>(Character->Controller) : Controller;
        if (Controller)
        {
            Controller->SetHUDScore(GetScore());
        }
    }
}

// �÷��̾� �й� Ƚ�� ����
void ALetsShootPlayerState::AddToDefeats(int32 DefeatsAmount)
{
    // �� �й� Ƚ���� DefeatsAmount�� �߰�
    Defeats += DefeatsAmount;
    // ĳ���͸� ��� HUD�� �й� Ƚ���� ����
    Character = Character == nullptr ? Cast<ALetsShootCharacter>(GetPawn()) : Character;
    if (Character)
    {
        Controller = Controller == nullptr ? Cast<ALetsShootPlayerController>(Character->Controller) : Controller;
        if (Controller)
        {
            Controller->SetHUDDefeats(Defeats);
        }
    }
}

// �й� Ƚ���� ����� �� HUD�� ������Ʈ
void ALetsShootPlayerState::OnRep_Defeats()
{
    // ĳ���͸� ��� HUD�� �й� Ƚ���� ����
    Character = Character == nullptr ? Cast<ALetsShootCharacter>(GetPawn()) : Character;
    if (Character)
    {
        Controller = Controller == nullptr ? Cast<ALetsShootPlayerController>(Character->Controller) : Controller;
        if (Controller)
        {
            Controller->SetHUDDefeats(Defeats);
        }
    }
}


// �÷��̾��� ���� �����ϴ� �޼���
void ALetsShootPlayerState::SetTeam(ETeam TeamToSet)
{
    // ���޹��� ������ �÷��̾��� ���� ����
    Team = TeamToSet;

    // �÷��̾��� ĳ���͸� ���, ���� ���� ������ ����
    ALetsShootCharacter* BCharacter = Cast <ALetsShootCharacter>(GetPawn());
    if (BCharacter)
    {
        // ����: Team - ������ ���� ������ Ÿ��
        BCharacter->SetTeamColor(Team);
    }
}

// �� ������ ����� �� ȣ��Ǵ� �޼���
void ALetsShootPlayerState::OnRep_Team()
{
    // �÷��̾��� ĳ���͸� ���, ���� ���� ������ ����
    ALetsShootCharacter* BCharacter = Cast <ALetsShootCharacter>(GetPawn());
    if (BCharacter)
    {
        // �� ������ �����ϴ� �κ�.
        BCharacter->SetTeamColor(Team);
    }
}


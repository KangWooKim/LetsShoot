# Announcement.h

이 코드는 게임에서 사용되는 공지사항과 관련된 문자열들을 상수로 정의한 네임스페이스인 Announcement를 선언합니다.



NewMatchStartsIn: 새로운 경기 시작 전 카운트다운을 표시할 때 사용됩니다.

ThereIsNoWinner: 승자가 없을 때 표시되는 문자열입니다.

YouAreTheWinner: 플레이어가 승리했을 때 표시되는 문자열입니다.

PlayersTiedForTheWin: 여러 플레이어가 동점으로 모두 승리했을 때 표시되는 문자열입니다.

TeamsTiedForTheWin: 두 팀이 동점으로 무승부일 때 표시되는 문자열입니다.

RedTeam: 빨간팀을 나타내는 문자열입니다.

BlueTeam: 파란팀을 나타내는 문자열입니다.

RedTeamWins: 빨간팀이 이겼을 때 표시되는 문자열입니다.

BlueTeamWins: 파란팀이 이겼을 때 표시되는 문자열입니다.

---

# CombatState.h

이 코드는 전투 상태를 나타내는 ECombatState 열거형을 정의합니다. 각 열거 상태는 다음과 같습니다.



ECS_Unoccupied: 전투가 진행되지 않고 아무런 작업도 수행하지 않는 상태를 나타냅니다.

ECS_Reloading: 캐릭터가 총알을 장전하는 중인 상태를 나타냅니다.

ECS_ThrowingGrenade: 캐릭터가 수류탄을 던지는 중인 상태를 나타냅니다.

ECS_SwappingWeapons: 캐릭터가 무기를 교체하는 중인 상태를 나타냅니다.

ECS_MAX: 열거형의 마지막 값으로, 기본 최댓값을 의미하는 데 사용됩니다. 이는 유효한 상태가 아니지만 열거형의 크기를 쉽게 확인할 수 있습니다.


---

# TeamColor.h

이 코드는 팀 정보를 나타내는 ETeamColor 열거형을 정의합니다. 각 열거 상태는 다음과 같습니다.



ET_RedTeam: 빨간팀을 나타냅니다.

ET_BlueTeam: 파란팀을 나타냅니다.

ET_NoTeam: 팀이 없음을 나타내는 상태입니다. 일반적으로 개별 플레이어나 팀 기반 게임에서 팀 배정이 되지 않은 경우를 나타냅니다.

ET_MAX: 열거형의 마지막 값으로, 기본 최댓값을 의미하는 데 사용됩니다. 이는 유효한 상태가 아니지만 열거형의 크기를 쉽게 확인할 수 있습니다.

---

# TurningInPlace.h

이 코드는 캐릭터의 제자리 회전 상태를 나타내는 ETurningInPlace 열거형을 정의합니다. 각 열거 상태는 다음과 같습니다.



ETIP_Left: 캐릭터가 왼쪽으로 제자리에서 회전하고 있는 상태를 나타냅니다.

ETIP_Right: 캐릭터가 오른쪽으로 제자리에서 회전하고 있는 상태를 나타냅니다.

ETIP_NotTurning: 캐릭터가 제자리에서 회전하지 않고 있는 상태를 나타냅니다.

ETIP_MAX: 열거형의 마지막 값으로, 기본 최댓값을 의미하는 데 사용됩니다. 이는 유효한 상태가 아니지만 열거형의 크기를 쉽게 확인할 수 있습니다.

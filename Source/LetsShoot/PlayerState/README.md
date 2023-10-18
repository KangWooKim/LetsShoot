# LetsShootPlayerState.h

### GetLifetimeReplicatedProps(TArray<FLifetimeProperty>&) const

네트워크에서 동기화되어야 할 변수를 리플리케이션 리스트에 추가

  
### AddToScore(float ScoreAmount)

점수를 증가시키고 HUD에 반영


### OnRep_Score()

점수의 변경 발생 시 HUD 점수 업데이트 처리


### AddToDefeats(int32 DefeatsAmount)

패배 횟수를 증가시키고 HUD에 반영


### OnRep_Defeats()

패배 횟수의 변경 발생 시 HUD 패배 횟수 업데이트 처리


### SetTeam(ETeamColor TeamToSet)

팀 설정 및 캐릭터에 팀 색상 반영


### OnRep_Team()

팀 변경 시 캐릭터에 팀 색상 반영 처리

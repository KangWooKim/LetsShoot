# LetsShootGameState.h

### void ALetsShootGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const: 
  이 메서드는 Unreal Engine의 네트워크 복제 시스템에 의해 호출되며, 어떤 프로퍼티들이 복제되어야 하는지 명시합니다. 이 경우, TopScoringPlayers, RedTeamScore, BlueTeamScore가 복제됩니다.

### void ALetsShootGameState::UpdateTopScore(class ALetsShootPlayerState* ScoringPlayer):
  이 메서드는 주어진 ScoringPlayer의 점수를 바탕으로 최고 점수를 업데이트합니다. 만약 ScoringPlayer의 점수가 현재 최고 점수와 같거나 더 높다면, ScoringPlayer를 최고 점수를 얻은 플레이어 목록에 추가하거나 목록을 갱신합니다.

### void ALetsShootGameState::RedTeamScores()와 void ALetsShootGameState::BlueTeamScores(): 
  이 메서드들은 각각 레드팀과 블루팀이 점수를 얻었을 때 호출됩니다. 팀의 점수를 증가시키고, 첫 번째 플레이어 컨트롤러에 해당 팀의 점수를 HUD에 업데이트하도록 요청합니다.

### void ALetsShootGameState::OnRep_RedTeamScore()와 void ALetsShootGameState::OnRep_BlueTeamScore(): 
  이 메서드들은 각각 레드팀과 블루팀의 점수가 복제된 후 호출됩니다. 첫 번째 플레이어 컨트롤러에 해당 팀의 점수를 HUD에 업데이트하도록 요청합니다.

이 클래스는 팀 기반 게임의 상태를 관리하는 데 사용되며, 팀의 점수와 최고 점수를 업데이트하는 기능을 제공합니다. 이때 Unreal Engine의 네트워크 복제 기능을 활용하여 모든 클라이언트에서 일관된 게임 상태를 유지합니다.

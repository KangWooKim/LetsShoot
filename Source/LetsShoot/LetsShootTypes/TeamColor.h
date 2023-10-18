#pragma once

UENUM(BlueprintType)
enum class ETeamColor : uint8
{
	ET_RedTeam UMETA(DisplayName = "Red Team"),  // ÆÀ: »¡°£ ÆÀ
	ET_BlueTeam UMETA(DisplayName = "Blue Team"),  // ÆÀ: ÆÄ¶õ ÆÀ
	ET_NoTeam UMETA(DisplayName = "No Team"),  // ÆÀ: ¹ÌÁöÁ¤

	ET_MAX UMETA(DisplayName = "Default MAX")  // ÆÀ: ±âº» ÃÖ´ë°ª
};

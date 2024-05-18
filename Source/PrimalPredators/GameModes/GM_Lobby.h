#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PrimalPredators/Characters/PredatorSettings.h"
#include "GM_Lobby.generated.h"

class APlayerController;
class UClass;

UCLASS()
class PRIMALPREDATORS_API AGM_Lobby : public AGameModeBase{
	public:
		AGM_Lobby();

	private:
		GENERATED_BODY()
	
};

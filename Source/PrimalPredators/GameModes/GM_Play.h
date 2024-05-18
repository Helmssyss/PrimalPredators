#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GM_Play.generated.h"

class ABasePredator;
class APC_Play;
class APlayerController;

UCLASS()
class PRIMALPREDATORS_API AGM_Play : public AGameModeBase{
	public:
		AGM_Play();

		virtual void SwapPlayerControllers(APlayerController* OldPC, APlayerController* NewPC) override;

		void RespawnPredator(APC_Play* PController, ABasePredator* Predator) const;
		void SpawnSelectedPredator(APC_Play* PController,UClass* PlayerClass);

	private:
		GENERATED_BODY()
};
